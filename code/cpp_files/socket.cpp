#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include "../h_files/socket.h"

int connectToServer() {
    SOCKET clientSocket;
    int port = 55555;
    WSADATA wsaData;
    int wsaerror;

    // --- STEP 1: Initialize WinSock ---
    WORD wVersionRequested = MAKEWORD(2, 2);
    wsaerror = WSAStartup(wVersionRequested, &wsaData);

    if (wsaerror != 0) {
        std::cout << "The Winsock DLL not found" << std::endl;
        return 0;
    } else {
        std::cout << "The Winsock DLL found" << std::endl;
        std::cout << "The status: " << wsaData.szSystemStatus << std::endl;
    }

    // --- STEP 2: Create the client socket ---
    // Same parameters as server — TCP over IPv4
    clientSocket = INVALID_SOCKET;
    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (clientSocket == INVALID_SOCKET) {
        std::cout << "Error at socket(): " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 0;
    } else {
        std::cout << "socket() is OK" << std::endl;
    }

    // --- STEP 3: Specify the server address to connect to ---
    // This is the server's IP and port — must match what server bound to
    sockaddr_in clientService;
    clientService.sin_family = AF_INET;
    InetPton(AF_INET, "127.0.0.1", &clientService.sin_addr.s_addr); // server is on localhost
    clientService.sin_port = htons(port);

    // --- STEP 4: Connect to the server ---
    // connect() reaches out to the server's IP:port and establishes the connection
    if (connect(clientSocket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR) {
        std::cout << "connect() failed: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 0;
    } else {
        std::cout << "connect() is OK - connected to server!" << std::endl;
        std::cout << "Client can start sending and receiving data" << std::endl;
    }

    // --- STEP 5: Cleanup ---
    closesocket(clientSocket);
    system("pause");
    WSACleanup();
    return 0;
}