#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include "../h_files/server.h"

int main2() {
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

    // --- STEP 6: Cleanup ---
    // Close both sockets and shut down WinSock
    closesocket(acceptSocket);
    closesocket(serverSocket);
    system("pause");
    WSACleanup();

    return 0;
}