#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include "../h_files/server.h"

int serverInitiation() {
    SOCKET serverSocket, acceptSocket;
    int port = 55555;
    WSADATA wsaData;
    int wsaerror;

    // --- STEP 1: Initialize WinSock ---
    // Request WinSock version 2.2
    WORD wVersionRequested = MAKEWORD(2, 2);
    wsaerror = WSAStartup(wVersionRequested, &wsaData);

    if (wsaerror != 0) {
        std::cout << "The Winsock DLL not found" << std::endl;
        return 0;
    } else {
        std::cout << "The Winsock DLL found" << std::endl;
        std::cout << "The status: " << wsaData.szSystemStatus << std::endl;
    }

    // --- STEP 2: Create the server socket ---
    // AF_INET = IPv4, SOCK_STREAM = TCP, IPPROTO_TCP = TCP protocol
    serverSocket = INVALID_SOCKET;
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (serverSocket == INVALID_SOCKET) {
        std::cout << "Error at socket(): " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 0;
    } else {
        std::cout << "socket() is OK" << std::endl;
    }

    // --- STEP 3: Bind the socket to an address and port ---
    // This tells the OS: "this socket belongs to port 55555 on this machine"
    sockaddr_in service;
    service.sin_family = AF_INET;
    InetPton(AF_INET, "127.0.0.1", &service.sin_addr.s_addr); // listen on localhost
    service.sin_port = htons(port); // htons converts port to correct byte order

    if (bind(serverSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR) {
        std::cout << "Error at bind(): " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 0;
    } else {
        std::cout << "bind() is OK" << std::endl;
    }

    // --- STEP 4: Listen for incoming connections ---
    // The '1' is the backlog — how many pending connections can queue up
    if (listen(serverSocket, 1) == SOCKET_ERROR)
        std::cout << "Error at listen(): " << WSAGetLastError() << std::endl;
    else
        std::cout << "listen() is OK - waiting for a client..." << std::endl;

    // --- STEP 5: Accept a client connection ---
    // accept() BLOCKS here until a client connects
    // Returns a NEW socket dedicated to this client — serverSocket keeps listening
    acceptSocket = accept(serverSocket, NULL, NULL);
    if (acceptSocket == INVALID_SOCKET) {
        std::cout << "Error at accept(): " << WSAGetLastError() << std::endl;
        WSACleanup();
        return -1;
    }
    std::cout << "accept() is OK - client connected!" << std::endl;

    char buffer[200];

    int byteRead = recv(acceptSocket, buffer, 200, 0);

    if (byteRead >= 0)
        std::cout << "Message received from the client: " << buffer << std::endl;
    else
        WSACleanup();

    char serverAnswer[200] = "Message from you was succesfully recieved!";
    int byteCount = send(acceptSocket, serverAnswer, 200, 0);

    if (byteCount >= 0)
        std::cout << "Answer to the client was sent" << std::endl;
    else
        WSACleanup();

    // --- STEP 6: Cleanup ---
    // Close both sockets and shut down WinSock
    closesocket(acceptSocket);
    closesocket(serverSocket);
    system("pause");
    WSACleanup();

    return 0;
}
// #include <winsock2.h>
// #include <ws2tcpip.h>
// #include <iostream>
// #include <thread>
// #include <mutex>
// #include <cstring>
// #include "../h_files/server.h"
// #include "../h_files/messages.h"
// #include "../h_files/net_utils.h"
// #include "../h_files/Definers.h"
//
// using namespace std;
//
// // ---------------------------------------------------------------
// // Shared game state — lives on the server, protected by a mutex.
// // Clients only hold a rendered view; the server is the single
// // source of truth for the board.
// // ---------------------------------------------------------------
// struct GameState {
//     // fields[] is row-major: fields[(row-1)*7 + (col-1)]
//     // Value matches Field::getSymbol(): ' ' unoccupied, 'U' black, '^' red
//     char    fields[42];
//     uint8_t current_player; // 1 = BLACK (goes first), 2 = RED
//     uint8_t game_over;      // 0=playing 1=P1wins 2=P2wins 3=draw 4=opponent_quit
//     mutex   mtx;
//
//     GameState() {
//         memset(fields, FIELD_UNOCCUPIED_SYMBOL, sizeof(fields));
//         current_player = 1;
//         game_over      = 0;
//     }
// };
//
// // ---------------------------------------------------------------
// // Board helpers
// // ---------------------------------------------------------------
//
// // Returns the field index for the lowest unoccupied row in 'col' (1-indexed).
// // Returns -1 if the column is full.
// static int lowestFreeRow(const char fields[42], int col) {
//     // Scan from bottom row (row 6) upward — row 1 is visually the top
//     // but in Connect 4 gravity fills from the bottom.
//     // Your FieldInitialization stores row 1 at top, row 6 at bottom,
//     // so "lowest" (gravity) means highest row number.
//     for (int row = ROWS_NUMBER; row >= 1; --row) {
//         int idx = (row - 1) * COLUMNS_NUMBER + (col - 1);
//         if (fields[idx] == FIELD_UNOCCUPIED_SYMBOL)
//             return idx;
//     }
//     return -1; // full
// }
//
// static bool columnValid(const char fields[42], int col) {
//     if (col < 1 || col > COLUMNS_NUMBER) return false;
//     return lowestFreeRow(fields, col) != -1;
// }
//
// static void applyMove(char fields[42], int col, uint8_t player_id) {
//     int idx = lowestFreeRow(fields, col);
//     fields[idx] = (player_id == 1) ? FIELD_OCCUPIED_SYMBOL_BLACK
//                                     : FIELD_OCCUPIED_SYMBOL_RED;
// }
//
// // Check 4-in-a-row for a given symbol — mirrors your victory() logic
// static bool checkWin(const char fields[42], char symbol) {
//     int dirs[4][2] = {{0,1},{1,0},{1,1},{1,-1}};
//     for (int row = 1; row <= ROWS_NUMBER; ++row) {
//         for (int col = 1; col <= COLUMNS_NUMBER; ++col) {
//             if (fields[(row-1)*COLUMNS_NUMBER+(col-1)] != symbol) continue;
//             for (auto& d : dirs) {
//                 int count = 1;
//                 for (int step = 1; step < 4; ++step) {
//                     int r = row + step*d[0];
//                     int c = col + step*d[1];
//                     if (r<1||r>ROWS_NUMBER||c<1||c>COLUMNS_NUMBER) break;
//                     if (fields[(r-1)*COLUMNS_NUMBER+(c-1)] != symbol) break;
//                     count++;
//                 }
//                 if (count == 4) return true;
//             }
//         }
//     }
//     return false;
// }
//
// static bool checkDraw(const char fields[42]) {
//     // Draw when row 1 (top row) has no empty slot in any column
//     for (int col = 1; col <= COLUMNS_NUMBER; ++col) {
//         if (fields[(1-1)*COLUMNS_NUMBER+(col-1)] == FIELD_UNOCCUPIED_SYMBOL)
//             return false;
//     }
//     return true;
// }
//
// // ---------------------------------------------------------------
// // Build a StateMsg from current game state (call while holding mtx)
// // ---------------------------------------------------------------
// static StateMsg buildStateMsg(const GameState& g) {
//     StateMsg msg;
//     msg.type = MSG_STATE;
//     memcpy(msg.fields, g.fields, 42);
//     msg.current_player = g.current_player;
//     msg.game_over      = g.game_over;
//     return msg;
// }
//
// // ---------------------------------------------------------------
// // Broadcast to both clients — call while holding mtx
// // ---------------------------------------------------------------
// static void broadcast(SOCKET s1, SOCKET s2, const StateMsg& msg) {
//     send_all(s1, &msg, sizeof(msg));
//     send_all(s2, &msg, sizeof(msg));
// }
//
// // ---------------------------------------------------------------
// // Per-client thread — one instance runs for each connected player
// // ---------------------------------------------------------------
// static void handlePlayer(SOCKET mySock, SOCKET otherSock,
//                          uint8_t myId, GameState& game)
// {
//     while (true) {
//         // Block until this client sends a MoveMsg
//         MoveMsg move{};
//         if (!recv_all(mySock, &move, sizeof(move))) {
//             // Connection lost — notify the other player
//             lock_guard<mutex> lock(game.mtx);
//             if (!game.game_over) {
//                 game.game_over = 4; // opponent quit
//                 StateMsg state = buildStateMsg(game);
//                 send_all(otherSock, &state, sizeof(state));
//                 cout << "[Server] Player " << (int)myId
//                      << " disconnected." << endl;
//             }
//             return;
//         }
//
//         if (move.type != MSG_MOVE) continue; // ignore unexpected types
//
//         lock_guard<mutex> lock(game.mtx);
//
//         // Reject move if game already over
//         if (game.game_over) return;
//
//         // Reject move if it's not this player's turn
//         if (game.current_player != myId) {
//             ErrorMsg err{ MSG_ERROR, ERR_NOT_YOUR_TURN };
//             send_all(mySock, &err, sizeof(err));
//             continue;
//         }
//
//         // Reject illegal column
//         if (!columnValid(game.fields, move.column)) {
//             ErrorMsg err{ MSG_ERROR, ERR_ILLEGAL_MOVE };
//             send_all(mySock, &err, sizeof(err));
//             continue;
//         }
//
//         // Apply the move
//         applyMove(game.fields, move.column, myId);
//         cout << "[Server] Player " << (int)myId
//              << " dropped in column " << (int)move.column << endl;
//
//         // Check win / draw
//         char sym = (myId == 1) ? FIELD_OCCUPIED_SYMBOL_BLACK
//                                 : FIELD_OCCUPIED_SYMBOL_RED;
//         if (checkWin(game.fields, sym)) {
//             game.game_over = myId; // 1 = P1 wins, 2 = P2 wins
//             cout << "[Server] Player " << (int)myId << " wins!" << endl;
//         } else if (checkDraw(game.fields)) {
//             game.game_over = 3;
//             cout << "[Server] Draw!" << endl;
//         }
//
//         // Advance turn
//         game.current_player = (myId == 1) ? 2 : 1;
//
//         // Broadcast updated state to both clients
//         StateMsg state = buildStateMsg(game);
//         broadcast(mySock, otherSock, state);
//
//         if (game.game_over) return;
//     }
// }
//
// // ---------------------------------------------------------------
// // serverInitiation — entry point called from server_main.cpp
// // ---------------------------------------------------------------
// int serverInitiation() {
//     WSADATA wsaData;
//     if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
//         cout << "WSAStartup failed" << endl;
//         return 0;
//     }
//     cout << "Winsock initialized. Status: " << wsaData.szSystemStatus << endl;
//
//     SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
//     if (serverSocket == INVALID_SOCKET) {
//         cout << "socket() failed: " << WSAGetLastError() << endl;
//         WSACleanup();
//         return 0;
//     }
//
//     // SO_REUSEADDR lets us restart the server immediately without
//     // waiting for the OS TIME_WAIT period to expire on port 55555
//     int reuse = 1;
//     setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR,
//                reinterpret_cast<const char*>(&reuse), sizeof(reuse));
//
//     sockaddr_in service{};
//     service.sin_family = AF_INET;
//     service.sin_addr.s_addr = INADDR_ANY; // accept on all interfaces, not just localhost
//     service.sin_port = htons(55555);
//
//     if (bind(serverSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR) {
//         cout << "bind() failed: " << WSAGetLastError() << endl;
//         closesocket(serverSocket);
//         WSACleanup();
//         return 0;
//     }
//     cout << "bind() OK" << endl;
//
//     // Backlog of 2 — we expect exactly 2 clients, queue up to 2 pending
//     if (listen(serverSocket, 2) == SOCKET_ERROR) {
//         cout << "listen() failed: " << WSAGetLastError() << endl;
//         closesocket(serverSocket);
//         WSACleanup();
//         return 0;
//     }
//     cout << "Listening on port 55555 — waiting for 2 players..." << endl;
//
//     // --- Accept player 1 ---
//     SOCKET sock1 = accept(serverSocket, NULL, NULL);
//     if (sock1 == INVALID_SOCKET) {
//         cout << "accept() P1 failed: " << WSAGetLastError() << endl;
//         WSACleanup();
//         return -1;
//     }
//     cout << "Player 1 connected!" << endl;
//
//     // Tell client 1 it is player 1 (BLACK, goes first)
//     AssignMsg assign1{ MSG_ASSIGN, 1 };
//     send_all(sock1, &assign1, sizeof(assign1));
//
//     // --- Accept player 2 ---
//     SOCKET sock2 = accept(serverSocket, NULL, NULL);
//     if (sock2 == INVALID_SOCKET) {
//         cout << "accept() P2 failed: " << WSAGetLastError() << endl;
//         closesocket(sock1);
//         WSACleanup();
//         return -1;
//     }
//     cout << "Player 2 connected! Game starting." << endl;
//
//     // Tell client 2 it is player 2 (RED, moves second)
//     AssignMsg assign2{ MSG_ASSIGN, 2 };
//     send_all(sock2, &assign2, sizeof(assign2));
//
//     // Send the initial (empty) board to both clients so they can render
//     GameState game;
//     StateMsg initial = buildStateMsg(game);
//     broadcast(sock1, sock2, initial);
//
//     // --- Spawn one thread per player ---
//     // Each thread blocks on recv() independently — this is the
//     // multithreading requirement. The mutex inside GameState ensures
//     // only one thread can modify the board at a time.
//     thread t1(handlePlayer, sock1, sock2, 1, ref(game));
//     thread t2(handlePlayer, sock2, sock1, 2, ref(game));
//
//     t1.join();
//     t2.join();
//
//     cout << "[Server] Game over. Cleaning up." << endl;
//
//     closesocket(sock1);
//     closesocket(sock2);
//     closesocket(serverSocket);
//     system("pause");
//     WSACleanup();
//     return 0;
// }
