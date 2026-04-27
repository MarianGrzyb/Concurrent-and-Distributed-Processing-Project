#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "../h_files/GameServer.h"
#include "../h_files/NetworkMessage.h"
#include "../h_files/Protocol.h"
#include "../h_files/Definers.h"
#include "../h_files/Field.h"
#include "../h_files/Player.h"
#include "../h_files/GameLogic.h"
#include "../h_files/FieldsInitialization.h"

using namespace std;

// ============================================================
//  acceptClients
//  Sequential — order matters (slot 0 = first connector).
// ============================================================
bool acceptClients(SOCKET listenSock, SOCKET clientSockets[2])
{
    cout << "[SERVER] Waiting for " << PROTOCOL_MAX_CLIENTS << " clients..." << endl;

    for (int i = 0; i < PROTOCOL_MAX_CLIENTS; ++i)
    {
        clientSockets[i] = accept(listenSock, NULL, NULL);
        if (clientSockets[i] == INVALID_SOCKET)
        {
            cerr << "[SERVER] accept() failed: " << WSAGetLastError() << endl;
            return false;
        }
        cout << "[SERVER] Client " << (i + 1) << " connected." << endl;
    }
    return true;
}

// ============================================================
//  negotiateNameForClient  (unchanged — per-client, no shared state)
// ============================================================
static string negotiateNameForClient(SOCKET sock, const string& defaultName)
{
    while (true)
    {
        if (!sendMessage(sock, buildAskName()))
            return defaultName;

        RawMessage resp;
        if (!recvMessage(sock, resp))
            return defaultName;

        if (resp.type == MSG_NAME_CHOICE)
        {
            string name = parseNameChoice(resp);
            if (name.empty())
                return defaultName;
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

// ============================================================
//  SetupContext
//  Shared state that both setup threads read/write.
//  Protected by mutex + condition_variable so client 2
//  cannot pick a colour until client 1 has committed theirs.
// ============================================================
struct SetupContext
{
    // --- shared colour registry ---
    mutex              colourMutex;
    condition_variable colourCV;
    vector<string>     takenColours;     // grows as clients commit
    int                coloursCommitted = 0; // how many slots filled so far

    // --- results written by each thread, read by main after join ---
    Player* players[2] = { nullptr, nullptr };
    bool    success[2] = { false,   false   };
};

// ============================================================
//  setupThreadFunc
//  Runs in a dedicated thread for each client.
//  Negotiates colour (with mutex), then name, then sends SETUP_DONE.
// ============================================================
static void setupThreadFunc(SetupContext& ctx, int idx, SOCKET sock)
{
    int    slot        = idx + 1;
    string defaultName = (slot == PLAYER_DEFAULT_ID_1)
                         ? PLAYER_DEFAULT_NAME_1
                         : PLAYER_DEFAULT_NAME_2;

    // ----------------------------------------------------------
    // COLOUR PHASE
    // ----------------------------------------------------------
    // Client 1 (idx=0): picks freely, commits immediately.
    // Client 2 (idx=1): waits until client 1 has committed (coloursCommitted >= 1), then sees which colour is taken.
    // This is the critical section that justifies the mutex.
    // ----------------------------------------------------------

    string chosenColour;

    while (true)
    {
        // Determine availability under the lock
        bool blackAvailable, redAvailable;
        {
            unique_lock<mutex> lock(ctx.colourMutex);

            // Client 2 must wait until client 1 has committed a colour
            ctx.colourCV.wait(lock, [&]{ return ctx.coloursCommitted >= idx; });

            blackAvailable = true;
            redAvailable   = true;
            for (const string& c : ctx.takenColours)
            {
                if (c == COLOUR_BLACK_NAME) blackAvailable = false;
                if (c == COLOUR_RED_NAME)   redAvailable   = false;
            }
        }

        // Send availability to client (outside lock — no shared data touched)
        if (!sendMessage(sock, buildChooseColour(blackAvailable, redAvailable)))
        {
            ctx.success[idx] = false;
            return;
        }

        RawMessage resp;
        if (!recvMessage(sock, resp) || resp.type != MSG_COLOUR_CHOICE)
        {
            ctx.success[idx] = false;
            return;
        }

        char keycap = parseColourChoice(resp);

        // Validate and commit under the lock
        {
            lock_guard<mutex> lock(ctx.colourMutex);

            bool valid = (keycap == COLOUR_BLACK_KEYCAP && blackAvailable)
                      || (keycap == COLOUR_RED_KEYCAP   && redAvailable);

            if (!valid)
            {
                sendMessage(sock, buildColourTaken());
                continue;
            }

            chosenColour = (keycap == COLOUR_BLACK_KEYCAP) ? COLOUR_BLACK_NAME : COLOUR_RED_NAME;

            ctx.takenColours.push_back(chosenColour);
            ctx.coloursCommitted++;
        }

        // Wake client 2 if it was waiting on client 1
        ctx.colourCV.notify_all();
        break;
    }

    // ----------------------------------------------------------
    // NAME PHASE  (fully independent — no shared state)
    // ----------------------------------------------------------
    string chosenName = negotiateNameForClient(sock, defaultName);
    if (chosenName.empty()) chosenName = defaultName;

    // ----------------------------------------------------------
    // CREATE PLAYER + SEND SETUP_DONE
    // ----------------------------------------------------------
    Player* p = new Player(slot, chosenColour);
    p->setName(chosenName);

    if (!sendMessage(sock, buildSetupDone(slot, chosenColour, chosenName)))
    {
        delete p;
        ctx.success[idx] = false;
        return;
    }

    cout << "[SERVER] Player " << slot
         << ", colour: " << chosenColour
         << ", name: "   << chosenName << endl;

    ctx.players[idx] = p;
    ctx.success[idx] = true;
}

// ============================================================
//  negotiatePlayers
//  Spawns one thread per client so both negotiate simultaneously.
//  A mutex + condition_variable ensures colour uniqueness.
// ============================================================
bool negotiatePlayers(SOCKET clientSockets[2], vector<Player*>& players)
{
    SetupContext ctx;

    // Launch one setup thread per client
    thread t0(setupThreadFunc, ref(ctx), 0, clientSockets[0]);
    thread t1(setupThreadFunc, ref(ctx), 1, clientSockets[1]);

    // Wait for both to finish
    t0.join();
    t1.join();

    if (!ctx.success[0] || !ctx.success[1])
    {
        cerr << "[SERVER] Setup failed for one or more clients." << endl;
        delete ctx.players[0];
        delete ctx.players[1];
        return false;
    }

    // Preserve slot order in the players vector
    players.push_back(ctx.players[0]);
    players.push_back(ctx.players[1]);
    return true;
}

// ============================================================
//  broadcastBoardState
//  Uses two threads to send to both clients simultaneously.
// ============================================================
void broadcastBoardState(SOCKET clientSockets[2],
                         const vector<Field*>& allFields,
                         int activeSlot)
{
    RawMessage msg = buildBoardState(allFields, activeSlot);

    // Each lambda captures msg by value — no shared mutable state, no mutex needed
    thread t0([msg, &clientSockets]() { sendMessage(clientSockets[0], msg); });
    thread t1([msg, &clientSockets]() { sendMessage(clientSockets[1], msg); });

    t0.join();
    t1.join();
}

// ============================================================
//  getValidatedMove
// ============================================================
int getValidatedMove(SOCKET clientSocket,
                     SOCKET /*otherClientSocket*/,
                     const vector<Field*>& allFields)
{
    if (!sendMessage(clientSocket, buildYourTurn()))
        return -1;

    while (true)
    {
        RawMessage response;
        if (!recvMessage(clientSocket, response))
            return -1;

        if (response.type == MSG_CLIENT_QUIT)
            return -1;

        if (response.type != MSG_COLUMN_CHOICE)
        {
            sendMessage(clientSocket, buildMoveInvalid());
            continue;
        }

        int column = parseColumnChoice(response);

        if (column < 1 || column > COLUMNS_NUMBER || !isColumnAvailable(allFields, column))
        {
            sendMessage(clientSocket, buildMoveInvalid());
            continue;
        }

        sendMessage(clientSocket, buildMoveOk());
        return column;
    }
}

// ============================================================
//  broadcastGameOver
// ============================================================
void broadcastGameOver(SOCKET clientSockets[2], char resultCode)
{
    RawMessage msg = buildGameOver(resultCode);
    thread t0([msg, &clientSockets]() { sendMessage(clientSockets[0], msg); });
    thread t1([msg, &clientSockets]() { sendMessage(clientSockets[1], msg); });
    t0.join();
    t1.join();
}

// ============================================================
//  playOneGame
// ============================================================
static bool playOneGame(SOCKET clientSockets[2], vector<Player*>& players)
{
    vector<Field*> allFields = initFields();

    int firstIdx  = (players[0]->getColour() == COLOUR_BLACK_NAME) ? 0 : 1;
    int secondIdx = 1 - firstIdx;

    int  turnCounter = 0;
    bool gameRunning = true;

    cout << "[SERVER] Game starting!" << endl;

    while (gameRunning)
    {
        turnCounter++;
        int activeIdx = (turnCounter % TURN_DETERMINANT == 0) ? secondIdx : firstIdx;

        int    activeSlot = players[activeIdx]->getId();
        SOCKET activeSock = clientSockets[activeIdx];
        SOCKET otherSock  = clientSockets[1 - activeIdx];

        broadcastBoardState(clientSockets, allFields, activeSlot);

        int column = getValidatedMove(activeSock, otherSock, allFields);

        if (column == -1)
        {
            char resultCode = (players[1 - activeIdx]->getId() == 1)
                              ? RESULT_WIN_PLAYER1
                              : RESULT_WIN_PLAYER2;
            broadcastGameOver(clientSockets, resultCode);
            gameRunning = false;
            break;
        }

        pawnPlacing(allFields, players[activeIdx]->getFieldSymbol(), column);

        if (victory(allFields, players[activeIdx]->getFieldSymbol()))
        {
            broadcastBoardState(clientSockets, allFields, activeSlot);

            char resultCode = (players[activeIdx]->getId() == 1)
                              ? RESULT_WIN_PLAYER1
                              : RESULT_WIN_PLAYER2;
            broadcastGameOver(clientSockets, resultCode);

            cout << "[SERVER] Player " << activeSlot
                 << " (" << players[activeIdx]->getName() << ") wins!" << endl;
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

    // Ask both clients simultaneously whether they want to play again
    thread t0([&clientSockets]() { sendMessage(clientSockets[0], buildPlayAgainPrompt()); });
    thread t1([&clientSockets]() { sendMessage(clientSockets[1], buildPlayAgainPrompt()); });
    t0.join();
    t1.join();

    cout << "[SERVER] Asking clients to play again..." << endl;

    int yesVotes = 0;
    for (int i = 0; i < PROTOCOL_MAX_CLIENTS; ++i)
    {
        RawMessage resp;
        if (!recvMessage(clientSockets[i], resp))
            return false;
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

// ============================================================
//  runGameServer
// ============================================================
int runGameServer()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        cerr << "[SERVER] WSAStartup failed." << endl;
        return 1;
    }

    SOCKET listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSock == INVALID_SOCKET)
    {
        cerr << "[SERVER] socket() failed: " << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in service;
    service.sin_family      = AF_INET;
    service.sin_addr.s_addr = htonl(INADDR_ANY);
    service.sin_port        = htons(PROTOCOL_PORT);

    if (bind(listenSock, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
    {
        cerr << "[SERVER] bind() failed: " << WSAGetLastError() << endl;
        closesocket(listenSock);
        WSACleanup();
        return 1;
    }

    if (listen(listenSock, PROTOCOL_LISTEN_BACKLOG) == SOCKET_ERROR)
    {
        cerr << "[SERVER] listen() failed: " << WSAGetLastError() << endl;
        closesocket(listenSock);
        WSACleanup();
        return 1;
    }

    cout << "[SERVER] Listening on port " << PROTOCOL_PORT << endl;

    SOCKET clientSockets[2] = { INVALID_SOCKET, INVALID_SOCKET };
    if (!acceptClients(listenSock, clientSockets))
    {
        closesocket(listenSock);
        WSACleanup();
        return 1;
    }

    bool keepPlaying = true;
    while (keepPlaying)
    {
        vector<Player*> players;
        if (!negotiatePlayers(clientSockets, players))
        {
            cerr << "[SERVER] Player negotiation failed." << endl;
            break;
        }

        keepPlaying = playOneGame(clientSockets, players);

        for (auto p : players) delete p;
    }

    for (int i = 0; i < PROTOCOL_MAX_CLIENTS; ++i)
        closesocket(clientSockets[i]);
    closesocket(listenSock);
    WSACleanup();
    return 0;
}