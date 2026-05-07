#include "server_interface.h"
#include <ws2tcpip.h>
#include "protocol.h"

#include <iostream>
#include <cstring>
#include <string>

/**
 * @file server_interface.cpp
 * @brief Реализация TCP-сервера ServerInterface на базе Winsock2.
 */

using namespace std;

/**
 * @brief Инициализирует Winsock2 и создаёт серверный сокет.
 * @throws std::runtime_error при ошибке WSAStartup или initSocket().
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
 * @brief Закрывает серверный сокет и вызывает WSACleanup().
 */
ServerInterface::~ServerInterface() {
    if (serverSocket != INVALID_SOCKET) {
        closesocket(serverSocket);
    }
    WSACleanup();
}

/**
 * @brief Создаёт TCP-сокет, устанавливает SO_REUSEADDR, bind() и listen().
 * @throws std::runtime_error если socket(), bind() или listen() вернул ошибку.
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
 * @brief Бесконечный цикл: accept() → handleClient() → следующий клиент.
 *
 * @details IP-адрес каждого подключившегося клиента выводится в stdout.
 * Ошибка accept() не завершает сервер — цикл продолжается.
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

        cout << "Client disconnected: " << clientIP << endl;
    }
}

/**
 * @brief Читает запросы клиента и отправляет ответы до разрыва соединения.
 *
 * @details Цикл завершается при recv() <= 0. Каждый буфер передаётся в
 * parseRequest(), результат — в ServerLogic::handleRequest().
 *
 * @param clientSocket Дескриптор сокета клиента, полученный от accept().
 */
void ServerInterface::handleClient(SOCKET clientSocket) {
    char buffer[1024];

    while (true) {
        memset(buffer, 0, sizeof(buffer));

        int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

        if (bytesRead <= 0) {
            break;
        }

        string raw(buffer, bytesRead);
        cout << "Received: " << raw;

        Request req = parseRequest(raw);
        string response = logic.handleRequest(req);

        cout << "Sending: " << response;

        send(clientSocket, response.c_str(), response.size(), 0);
    }

    closesocket(clientSocket);
}
