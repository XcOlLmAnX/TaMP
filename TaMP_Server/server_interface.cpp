#define _HAS_STD_BYTE 0

#include <winsock2.h>
#include <ws2tcpip.h>

#include "server_interface.h"
#include "protocol.h"

#include <iostream>
#include <cstring>
#include <string>

/**
 * @file server_interface.cpp
 * @brief Реализация сетевого интерфейса сервера.
 */

using namespace std;

/**
 * @brief Конструктор сервера.
 * Инициализирует Winsock и сокет.
 * @param port Порт для прослушивания.
 * @param logic Ссылка на бизнес-логику.
 */
ServerInterface::ServerInterface(int port, ServerLogic& logic)
    : port(port), serverSocket(INVALID_SOCKET), logic(logic)
{
    WSADATA wsaData;
    int rc = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (rc != 0) {
        throw runtime_error("WSAStartup failed: " + to_string(rc));
    }

    initSocket();
}

/**
 * @brief Деструктор.
 * Закрывает сокет и завершает работу Winsock.
 */
ServerInterface::~ServerInterface() {
    if (serverSocket != INVALID_SOCKET) {
        closesocket(serverSocket);
    }
    WSACleanup();
}

/**
 * @brief Инициализирует серверный сокет.
 */
void ServerInterface::initSocket() {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        throw runtime_error("socket() failed");
    }

    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR,
               reinterpret_cast<const char*>(&opt), sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(serverSocket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR) {
        throw runtime_error("bind() failed");
    }

    if (listen(serverSocket, 5) == SOCKET_ERROR) {
        throw runtime_error("listen() failed");
    }

    cout << "Server running on " << port << endl;
}

/**
 * @brief Запускает сервер и принимает подключения.
 */
void ServerInterface::run() {
    while (true) {
        sockaddr_in clientAddr{};
        int clientLen = sizeof(clientAddr);

        SOCKET clientSocket = accept(serverSocket,
                                     reinterpret_cast<sockaddr*>(&clientAddr),
                                     &clientLen);

        if (clientSocket == INVALID_SOCKET) {
            cerr << "accept() failed" << endl;
            continue;
        }

        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);

        cout << "Client connected: " << clientIP << endl;

        handleClient(clientSocket);

        cout << "Client disconected: " << clientIP << endl;
    }
}

/**
 * @brief Обрабатывает подключенного клиента.
 * @param clientSocket Сокет клиента.
 */
void ServerInterface::handleClient(int clientSocket) {
    char buffer[1024];

    while (true) {
        memset(buffer, 0, sizeof(buffer));

        int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

        if (bytesRead <= 0) {
            break;
        }

        string raw(buffer, bytesRead);
        cout << "Recieved: " << raw;

        Request req = parseRequest(raw);
        string response = logic.handleRequest(req);

        cout << "Sending: " << response;

        send(clientSocket, response.c_str(), response.size(), 0);
    }

    closesocket(clientSocket);
}