#define _HAS_STD_BYTE 0

#include <winsock2.h>
#include <ws2tcpip.h>

#include "server_interface.h"
#include "protocol.h"

#include <iostream>
#include <cstring>
#include <string>


ServerInterface::ServerInterface(int port, ServerLogic& logic)
    : port(port), serverSocket(INVALID_SOCKET), logic(logic)
{
    WSADATA wsaData;
    int rc = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (rc != 0) {
        throw std::runtime_error("WSAStartup failed: " + std::to_string(rc));
    }

    initSocket();
}

ServerInterface::~ServerInterface() {
    if (serverSocket != INVALID_SOCKET) {
        closesocket(serverSocket);
    }
    WSACleanup();
}

void ServerInterface::initSocket() {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        throw std::runtime_error("socket() failed");
    }

    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR,
               reinterpret_cast<const char*>(&opt), sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(serverSocket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR) {
        throw std::runtime_error("bind() failed");
    }

    if (listen(serverSocket, 5) == SOCKET_ERROR) {
        throw std::runtime_error("listen() failed");
    }

    std::cout << "Server running on " << port << std::endl;
}

void ServerInterface::run() {
    while (true) {
        sockaddr_in clientAddr{};
        int clientLen = sizeof(clientAddr);

        SOCKET clientSocket = accept(serverSocket,
                                     reinterpret_cast<sockaddr*>(&clientAddr),
                                     &clientLen);

        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "accept() failed" << std::endl;
            continue;
        }

        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);

        std::cout << "Client connected: " << clientIP << std::endl;

        handleClient(clientSocket);

        std::cout << "Client disconected: " << clientIP << std::endl;
    }
}

void ServerInterface::handleClient(int clientSocket) {
    char buffer[1024];

    while (true) {
        memset(buffer, 0, sizeof(buffer));

        int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

        if (bytesRead <= 0) {
            break;
        }

        std::string raw(buffer, bytesRead);
        std::cout << "Recieved: " << raw;

        Request req = parseRequest(raw);
        std::string response = logic.handleRequest(req);

        std::cout << "Sending: " << response;

        send(clientSocket, response.c_str(), response.size(), 0);
    }

    closesocket(clientSocket);
}