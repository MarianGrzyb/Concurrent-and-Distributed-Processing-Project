#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include "../h_files/server.h"

int serverInitiation() {
    SOCKET serverSocket, acceptSocket;
    int port = 55555;
    WSADATA wsaData;
    int wsaerror;

    // Initialize WinSock
    WORD wVersionRequested = MAKEWORD(2, 2);
    wsaerror = WSAStartup(wVersionRequested, &wsaData);

    if (wsaerror != 0) {
        std::cout << "The Winsock DLL not found" << std::endl;
        return 0;
    } else {
        std::cout << "The Winsock DLL found" << std::endl;
        std::cout << "The status: " << wsaData.szSystemStatus << std::endl;
    }

    // Create the server socket
    serverSocket = INVALID_SOCKET;
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (serverSocket == INVALID_SOCKET) {
        std::cout << "Error at socket(): " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 0;
    } else {
        std::cout << "socket() is OK" << std::endl;
    }

    // Bind the socket to an address and port
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

    // Listen for incoming connections
    if (listen(serverSocket, 1) == SOCKET_ERROR)
        std::cout << "Error at listen(): " << WSAGetLastError() << std::endl;
    else
        std::cout << "listen() is OK - waiting for a client..." << std::endl;

    // Accept a client connection
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

    // Cleanup
    closesocket(acceptSocket);
    closesocket(serverSocket);
    system("pause");
    WSACleanup();

    return 0;
}