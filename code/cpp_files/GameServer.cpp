// ============================================================
//  GameServer.cpp
// ============================================================

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <vector>
#include <string>

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
//  negotiateColourForClient
// ============================================================
static string negotiateColourForClient(SOCKET sock, const vector<string>& takenColours)
{
    bool blackAvailable = true;
    bool redAvailable   = true;
    for (const string& c : takenColours)
    {
        if (c == COLOUR_BLACK_NAME) blackAvailable = false;
        if (c == COLOUR_RED_NAME)   redAvailable   = false;
    }

    while (true)
    {
        if (!sendMessage(sock, buildChooseColour(blackAvailable, redAvailable)))
            return "";

        RawMessage resp;
        if (!recvMessage(sock, resp) || resp.type != MSG_COLOUR_CHOICE)
            return "";

        char keycap = parseColourChoice(resp);

        if (keycap == COLOUR_BLACK_KEYCAP && blackAvailable) return COLOUR_BLACK_NAME;
        if (keycap == COLOUR_RED_KEYCAP   && redAvailable)   return COLOUR_RED_NAME;

        if (!sendMessage(sock, buildColourTaken()))
            return "";
    }
}

// ============================================================
//  negotiateNameForClient
// ============================================================
static string negotiateNameForClient(SOCKET sock, const string& defaultName)
{
    while (true)
    {
        if (!sendMessage(sock, buildAskName()))
            return "";

        RawMessage resp;
        if (!recvMessage(sock, resp))
            return "";

        if (resp.type == MSG_NAME_CHOICE)
        {
            string name = parseNameChoice(resp);
            if (name.empty())
                return defaultName;
            if ((int)name.size() > PLAYER_NAME_CHANGE_NAME_MAX_LENGTH)
            {
                if (!sendMessage(sock, buildNameInvalid()))
                    return "";
                continue;
            }
            return name;
        }
        return defaultName;
    }
}

// ============================================================
//  negotiatePlayers
// ============================================================
bool negotiatePlayers(SOCKET clientSockets[2], vector<Player*>& players)
{
    vector<string> takenColours;

    for (int i = 0; i < PROTOCOL_MAX_CLIENTS; ++i)
    {
        int slot = i + 1;

        string colour = negotiateColourForClient(clientSockets[i], takenColours);
        if (colour.empty())
        {
            cerr << "[SERVER] Colour negotiation failed for client " << slot << endl;
            return false;
        }
        takenColours.push_back(colour);

        string defaultName = (slot == PLAYER_DEFAULT_ID_1)
                             ? PLAYER_DEFAULT_NAME_1
                             : PLAYER_DEFAULT_NAME_2;
        string name = negotiateNameForClient(clientSockets[i], defaultName);
        if (name.empty()) name = defaultName;

        Player* p = new Player(slot, colour);
        p->setName(name);
        players.push_back(p);

        if (!sendMessage(clientSockets[i], buildSetupDone(slot, colour, name)))
            return false;

        cout << "[SERVER] Player " << slot
             << " | colour: " << colour
             << " | name: "   << name << endl;
    }
    return true;
}

// ============================================================
//  broadcastBoardState
// ============================================================
void broadcastBoardState(SOCKET clientSockets[2],
                         const vector<Field*>& allFields,
                         int activeSlot)
{
    RawMessage msg = buildBoardState(allFields, activeSlot);
    for (int i = 0; i < PROTOCOL_MAX_CLIENTS; ++i)
    {
        if (!sendMessage(clientSockets[i], msg))
            cerr << "[SERVER] broadcastBoardState: send to client " << (i+1) << " failed." << endl;
    }
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
    for (int i = 0; i < PROTOCOL_MAX_CLIENTS; ++i)
        sendMessage(clientSockets[i], msg);
}

// ============================================================
//  playOneGame
//  Mirrors mainGameLoop() + startNewGame() from game.cpp.
//  Returns true if both clients want to play again, false otherwise.
// ============================================================
static bool playOneGame(SOCKET clientSockets[2], vector<Player*>& players)
{
    // --- initFields() mirrors startNewGame() calling initFields() ---
    vector<Field*> allFields = initFields();

    // --- determinePlayerOrder(): BLACK always goes first ---
    int firstIdx  = (players[0]->getColour() == COLOUR_BLACK_NAME) ? 0 : 1;
    int secondIdx = 1 - firstIdx;

    // --- mainGameLoop() turn counter ---
    int  turnCounter = 0;
    bool gameRunning = true;

    cout << "[SERVER] Game starting!" << endl;

    while (gameRunning)
    {
        // Mirror mainGameLoop:  turnCounter += 1; even -> second, odd -> first
        turnCounter++;
        int activeIdx = (turnCounter % TURN_DETERMINANT == 0) ? secondIdx : firstIdx;

        int    activeSlot = players[activeIdx]->getId();
        SOCKET activeSock = clientSockets[activeIdx];
        SOCKET otherSock  = clientSockets[1 - activeIdx];

        // displayBoard equivalent: broadcast state to both clients
        broadcastBoardState(clientSockets, allFields, activeSlot);

        // turn() equivalent: get a validated move from the active client
        int column = getValidatedMove(activeSock, otherSock, allFields);

        if (column == -1)
        {
            // Player quit mid-game — opponent wins
            char resultCode = (players[1 - activeIdx]->getId() == 1)
                              ? RESULT_WIN_PLAYER1
                              : RESULT_WIN_PLAYER2;
            broadcastGameOver(clientSockets, resultCode);
            gameRunning = false;
            break;
        }

        // pawnPlacing()
        pawnPlacing(allFields, players[activeIdx]->getFieldSymbol(), column);

        // victory() check — mirrors the if(victory(...)) block in mainGameLoop
        if (victory(allFields, players[activeIdx]->getFieldSymbol()))
        {
            // Send final board before game-over (mirrors displayBoard before win msg)
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

        // draw check
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

    // --- Ask both clients if they want to play again (mirrors main() do-while loop) ---
    cout << "[SERVER] Asking clients to play again..." << endl;

    for (int i = 0; i < PROTOCOL_MAX_CLIENTS; ++i)
        sendMessage(clientSockets[i], buildPlayAgainPrompt());

    int yesVotes = 0;
    for (int i = 0; i < PROTOCOL_MAX_CLIENTS; ++i)
    {
        RawMessage resp;
        if (!recvMessage(clientSockets[i], resp))
            return false;

        if (resp.type == MSG_PLAY_AGAIN_YES)
            yesVotes++;
    }

    // Both must agree to play again
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
//  Outer loop mirrors game.cpp main() do-while with mainMenu().
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

    // --- Outer loop: mirrors game.cpp main() do { mainMenu -> startNewGame } while(true) ---
    bool keepPlaying = true;
    while (keepPlaying)
    {
        // Fresh player setup each game (colour + name), mirrors startNewGame -> initPlayers
        vector<Player*> players;
        if (!negotiatePlayers(clientSockets, players))
        {
            cerr << "[SERVER] Player negotiation failed." << endl;
            break;
        }

        // Run one full game; returns true if both want to play again
        keepPlaying = playOneGame(clientSockets, players);

        for (auto p : players) delete p;
    }

    // ---- Cleanup ----
    for (int i = 0; i < PROTOCOL_MAX_CLIENTS; ++i)
        closesocket(clientSockets[i]);
    closesocket(listenSock);
    WSACleanup();
    return 0;
}
