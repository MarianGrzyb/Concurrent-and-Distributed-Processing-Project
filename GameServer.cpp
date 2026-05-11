#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

#include "GameServer.h"
#include "NetworkMessage.h"
#include "Protocol.h"
#include "Definers.h"
#include "Field.h"
#include "Player.h"
#include "GameLogic.h"
#include "FieldsInitialization.h"

using namespace std;

//  Connection state per client slot
enum class ConnState { CONNECTED, QUIT, DROPPED };

//  safeSend / safeRecv
//  Return false on any error without spamming cerr during expected disconnects.
static bool safeSend(SOCKET sock, const RawMessage& msg)
{
    if (sock == INVALID_SOCKET) return false;
    return sendMessage(sock, msg);
}

static bool safeRecv(SOCKET sock, RawMessage& msg)
{
    if (sock == INVALID_SOCKET) return false;
    return recvMessage(sock, msg);
}

//  classifyDisconnect
static ConnState classifyDisconnect()
{
    int err = WSAGetLastError();
    // 0 = clean close (FIN), 10054 = connection reset — both mean terminal closed
    (void)err;
    return ConnState::DROPPED;
}

//  waitForReconnect
//  Opens a temporary accept on RECONNECT_PORT, waits up to RECONNECT_TIMEOUT_SECONDS for the dropped client to come back.
//  The client must send MSG_RECONNECT_HELLO with its slot number. Returns the new SOCKET on success, INVALID_SOCKET on timeout.
static SOCKET waitForReconnect(int expectedSlot)
{
    cout << "[SERVER] Waiting " << RECONNECT_TIMEOUT_SECONDS << "s for Player " << expectedSlot << " to reconnect...\n";

    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET) return INVALID_SOCKET;

    int opt = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(RECONNECT_PORT);

    if (bind(s, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
    {
        closesocket(s);
        return INVALID_SOCKET;
    }

    listen(s, 1);

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(s, &fds);

    timeval tv{};
    tv.tv_sec = RECONNECT_TIMEOUT_SECONDS;
    tv.tv_usec = 0;

    int r = select(0, &fds, nullptr, nullptr, &tv);

    if (r <= 0)
    {
        cout << "[SERVER] reconnect timeout\n";
        closesocket(s);
        return INVALID_SOCKET;
    }

    SOCKET client = accept(s, nullptr, nullptr);
    closesocket(s);

    if (client == INVALID_SOCKET)
        return INVALID_SOCKET;

    RawMessage msg;
    if (!recvMessage(client, msg) || msg.type != MSG_RECONNECT_HELLO)
    {
        closesocket(client);
        return INVALID_SOCKET;
    }

    int slot = parseReconnectHello(msg);

    if (slot != expectedSlot)
    {
        closesocket(client);
        return INVALID_SOCKET;
    }

    cout << "[SERVER] Player " << expectedSlot << " reconnected\n";
    return client;
}

//  handleDroppedClient
//  Notifies the other client, opens reconnect window.
//  Returns true if the client reconnected.
static bool handleDroppedClient(SOCKET clientSockets[2], int droppedIdx)
{
    int otherIdx = 1 - droppedIdx;

    // Tell the other client that the opponent dropped (not quit)
    safeSend(clientSockets[otherIdx], buildOpponentDisconnected());

    // Close the dead socket
    closesocket(clientSockets[droppedIdx]);
    clientSockets[droppedIdx] = INVALID_SOCKET;

    // Wait for reconnect
    int expectedSlot = droppedIdx + 1;

    SOCKET newSock = waitForReconnect(expectedSlot);

    if (newSock == INVALID_SOCKET)
    {
        // No reconnect — tell the other client
        safeSend(clientSockets[otherIdx], buildReconnectFailed());
        return false;
    }

    // Reconnected — update socket and tell the other client
    clientSockets[droppedIdx] = newSock;
    safeSend(clientSockets[otherIdx], buildReconnectSuccess());
    return true;
}

//  acceptClients
bool acceptClients(SOCKET listenSock, SOCKET clientSockets[2])
{
    cout << "[SERVER] Waiting for " << PROTOCOL_MAX_CLIENTS << " clients..." << endl;

    for (int i = 0; i < PROTOCOL_MAX_CLIENTS; ++i)
    {
        clientSockets[i] = accept(listenSock, NULL, NULL);

        sendMessage(clientSockets[i], buildNormalConnect());

        if (clientSockets[i] == INVALID_SOCKET)
        {
            cerr << "[SERVER] accept() failed: " << WSAGetLastError() << endl;
            return false;
        }
        cout << "[SERVER] Client " << (i + 1) << " connected." << endl;
    }
    return true;
}

//  Setup phase
static string negotiateNameForClient(SOCKET sock, const string& defaultName)
{
    while (true)
    {
        if (!sendMessage(sock, buildAskName())) return defaultName;
        RawMessage resp;
        if (!recvMessage(sock, resp))           return defaultName;
        if (resp.type == MSG_NAME_CHOICE)
        {
            string name = parseNameChoice(resp);
            if (name.empty()) return defaultName;
            if ((int)name.size() > PLAYER_NAME_CHANGE_NAME_MAX_LENGTH)
            {
                sendMessage(sock, buildNameInvalid());
                continue;
            }
            return name;
        }
        return defaultName;
    }
}

struct SetupContext
{
    mutex colourMutex;
    condition_variable colourCV;
    vector<string> takenColours;
    int coloursCommitted = 0;
    Player* players[2]  = { nullptr, nullptr };
    bool success[2]  = { false,   false   };
};

static void setupThreadFunc(SetupContext& ctx, int idx, SOCKET sock)
{
    int slot = idx + 1;
    string defaultName = (slot == PLAYER_DEFAULT_ID_1) ? PLAYER_DEFAULT_NAME_1 : PLAYER_DEFAULT_NAME_2;
    string chosenColour;

    while (true)
    {
        bool blackAvailable, redAvailable;
        {
            unique_lock<mutex> lock(ctx.colourMutex);
            ctx.colourCV.wait(lock, [&]{ return ctx.coloursCommitted >= idx; });
            blackAvailable = redAvailable = true;
            for (const string& c : ctx.takenColours)
            {
                if (c == COLOUR_BLACK_NAME)
                    blackAvailable = false;
                if (c == COLOUR_RED_NAME)
                    redAvailable   = false;
            }
        }

        if (!sendMessage(sock, buildChooseColour(blackAvailable, redAvailable))) {
            ctx.success[idx] = false;
            return;
        }

        RawMessage resp;
        if (!recvMessage(sock, resp) || resp.type != MSG_COLOUR_CHOICE) {
            ctx.success[idx] = false;
            return;
        }

        char keycap = parseColourChoice(resp);
        {
            lock_guard<mutex> lock(ctx.colourMutex);
            bool valid = (keycap == COLOUR_BLACK_KEYCAP && blackAvailable) || (keycap == COLOUR_RED_KEYCAP   && redAvailable);
            if (!valid) {
                sendMessage(sock, buildColourTaken());
                continue;
            }
            chosenColour = (keycap == COLOUR_BLACK_KEYCAP) ? COLOUR_BLACK_NAME : COLOUR_RED_NAME;
            ctx.takenColours.push_back(chosenColour);
            ctx.coloursCommitted++;
        }
        ctx.colourCV.notify_all();
        break;
    }

    string chosenName = negotiateNameForClient(sock, defaultName);
    if (chosenName.empty())
        chosenName = defaultName;

    Player* p = new Player(slot, chosenColour);
    p->setName(chosenName);

    if (!sendMessage(sock, buildSetupDone(slot, chosenColour, chosenName))) {
        delete p;
        ctx.success[idx] = false;
        return;
    }

    cout << "[SERVER] Player " << slot  << ", colour: " << chosenColour << ", name: "   << chosenName << endl;

    ctx.players[idx] = p;
    ctx.success[idx] = true;
}

bool negotiatePlayers(SOCKET clientSockets[2], vector<Player*>& players)
{
    SetupContext ctx{};
    ctx.coloursCommitted = 0;
    ctx.takenColours.clear();
    thread t0(setupThreadFunc, ref(ctx), 0, clientSockets[0]);
    thread t1(setupThreadFunc, ref(ctx), 1, clientSockets[1]);
    t0.join();
    t1.join();

    if (!ctx.success[0] || !ctx.success[1])
    {
        cerr << "[SERVER] Setup failed." << endl;
        delete ctx.players[0]; delete ctx.players[1];
        return false;
    }
    players.push_back(ctx.players[0]);
    players.push_back(ctx.players[1]);
    return true;
}

//  broadcastBoardState
void broadcastBoardState(SOCKET clientSockets[2], const vector<Field*>& allFields, int activeSlot)
{
    RawMessage msg = buildBoardState(allFields, activeSlot);
    thread t0([msg, &clientSockets](){ safeSend(clientSockets[0], msg); });
    thread t1([msg, &clientSockets](){ safeSend(clientSockets[1], msg); });
    t0.join();
    t1.join();
}

//  broadcastGameOver
void broadcastGameOver(SOCKET clientSockets[2], char resultCode)
{
    RawMessage msg = buildGameOver(resultCode);
    thread t0([msg, &clientSockets](){ safeSend(clientSockets[0], msg); });
    thread t1([msg, &clientSockets](){ safeSend(clientSockets[1], msg); });
    t0.join();
    t1.join();
}

//  getValidatedMove
//  Now distinguishes Q (quit) from socket error (dropped).
//  Returns column on success, -1 on quit, -2 on drop.
int getValidatedMove(SOCKET clientSocket, SOCKET /*otherClientSocket*/, const vector<Field*>& allFields)
{
    if (!safeSend(clientSocket, buildYourTurn()))
        return -2;   // send failed = dropped

    while (true)
    {
        RawMessage response;
        if (!safeRecv(clientSocket, response))
            return -2;  // recv failed = terminal closed / network lost

        if (response.type == MSG_CLIENT_QUIT)
            return -1;  // player pressed Q

        if (response.type != MSG_COLUMN_CHOICE)
        {
            safeSend(clientSocket, buildMoveInvalid());
            continue;
        }

        int column = parseColumnChoice(response);
        if (column < 1 || column > COLUMNS_NUMBER || !isColumnAvailable(allFields, column))
        {
            safeSend(clientSocket, buildMoveInvalid());
            continue;
        }

        safeSend(clientSocket, buildMoveOk());
        return column;
    }
}

//  playOneGame
static bool playOneGame(SOCKET clientSockets[2], vector<Player*>& players)
{
    vector<Field*> allFields = initFields();

    int firstIdx = (players[0]->getColour() == COLOUR_BLACK_NAME) ? 0 : 1;
    int secondIdx = 1 - firstIdx;
    int turnCounter = 0;
    bool gameRunning = true;

    cout << "[SERVER] Game starting!" << endl;

    while (gameRunning)
    {
        turnCounter++;
        int activeIdx = (turnCounter % TURN_DETERMINANT == 0) ? secondIdx : firstIdx;
        int passiveIdx = 1 - activeIdx;
        int activeSlot = players[activeIdx]->getId();

        broadcastBoardState(clientSockets, allFields, activeSlot);

        int column = getValidatedMove(clientSockets[activeIdx], clientSockets[passiveIdx], allFields);

        if (column == -1)
        {
            // ---- Player pressed Q ----
            cout << "[SERVER] Player " << activeSlot << " quit." << endl;
            char resultCode = (players[passiveIdx]->getId() == 1) ? RESULT_WIN_PLAYER1 : RESULT_WIN_PLAYER2;
            // Tell the passive client the opponent quit cleanly
            safeSend(clientSockets[passiveIdx], buildOpponentQuit());
            broadcastGameOver(clientSockets, resultCode);
            gameRunning = false;
            break;
        }

        if (column == -2)
        {
            // ---- Terminal closed / network lost ----
            cout << "[SERVER] Player " << activeSlot << " dropped." << endl;
            bool reconnected = handleDroppedClient(clientSockets, activeIdx);

            if (!reconnected)
            {
                // No reconnect within timeout — opponent wins
                char resultCode = (players[passiveIdx]->getId() == 1) ? RESULT_WIN_PLAYER1 : RESULT_WIN_PLAYER2;
                broadcastGameOver(clientSockets, resultCode);
                gameRunning = false;
                break;
            }

            // Reconnected — resend the board and retry the same turn
            broadcastBoardState(clientSockets, allFields, activeSlot);
            continue;
        }

        pawnPlacing(allFields, players[activeIdx]->getFieldSymbol(), column);

        if (victory(allFields, players[activeIdx]->getFieldSymbol()))
        {
            broadcastBoardState(clientSockets, allFields, activeSlot);
            char resultCode = (players[activeIdx]->getId() == 1) ? RESULT_WIN_PLAYER1 : RESULT_WIN_PLAYER2;
            broadcastGameOver(clientSockets, resultCode);
            cout << "[SERVER] Player " << activeSlot << " (" << players[activeIdx]->getName() << ") wins!" << endl;
            gameRunning = false;
            break;
        }

        if (getAvailableColumns(allFields).empty())
        {
            broadcastBoardState(clientSockets, allFields, activeSlot);
            broadcastGameOver(clientSockets, RESULT_DRAW);
            cout << "[SERVER] Draw!" << endl;
            gameRunning = false;
            break;
        }
    }

    for (auto f : allFields) delete f;

    // ---- Ask both clients about playing again (parallel) ----
    {
        thread t0([&clientSockets](){ safeSend(clientSockets[0], buildPlayAgainPrompt()); });
        thread t1([&clientSockets](){ safeSend(clientSockets[1], buildPlayAgainPrompt()); });
        t0.join();
        t1.join();
    }

    cout << "[SERVER] Asking clients to play again..." << endl;

    int yesVotes = 0;
    for (int i = 0; i < PROTOCOL_MAX_CLIENTS; ++i)
    {
        RawMessage resp;
        if (!safeRecv(clientSockets[i], resp))
            continue;
        if (resp.type == MSG_PLAY_AGAIN_YES)
            yesVotes++;
    }

    if (yesVotes == PROTOCOL_MAX_CLIENTS)
    {
        cout << "[SERVER] Both players want to play again!" << endl;
        return true;
    }
    cout << "[SERVER] A player declined. Ending session." << endl;
    return false;
}

//  runGameServer
int runGameServer()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "[SERVER] WSAStartup failed." << endl;
        return 1;
    }

    SOCKET listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSock == INVALID_SOCKET) {
        cerr << "[SERVER] socket() failed." << endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in service;
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = htonl(INADDR_ANY);
    service.sin_port = htons(PROTOCOL_PORT);

    if (bind(listenSock, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR ||
        listen(listenSock, PROTOCOL_LISTEN_BACKLOG) == SOCKET_ERROR)
    {
        cerr << "[SERVER] bind/listen failed." << endl;
        closesocket(listenSock);
        WSACleanup();
        return 1;
    }

    cout << "[SERVER] Listening on port " << PROTOCOL_PORT << endl;

    SOCKET clientSockets[2] = { INVALID_SOCKET, INVALID_SOCKET };
    if (!acceptClients(listenSock, clientSockets)) {
        closesocket(listenSock);
        WSACleanup();
        return 1; }

    bool keepPlaying = true;
    while (keepPlaying)
    {
        vector<Player*> players;
        if (!negotiatePlayers(clientSockets, players)) {
            cerr << "[SERVER] Negotiation failed." << endl;
            break;
        }

        keepPlaying = playOneGame(clientSockets, players);
        for (auto p : players) delete p;
    }

    for (int i = 0; i < PROTOCOL_MAX_CLIENTS; ++i) {
        if (clientSockets[i] != INVALID_SOCKET)
            closesocket(clientSockets[i]);
    }

    closesocket(listenSock);
    WSACleanup();
    return 0;
}