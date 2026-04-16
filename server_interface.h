#pragma once

#include "server_logic.h"

/**
 * @file server_interface.h
 * @brief View слой MVC — сетевой интерфейс TCP сервера.
 */

class ServerInterface {
public:
    ServerInterface(int port, ServerLogic& logic);
    ~ServerInterface();

    void run();

private:
    int port;
    int serverSocket;
    ServerLogic& logic;

    void initSocket();
    void handleClient(int clientSocket);
};