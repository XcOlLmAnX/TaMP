#ifndef TAMP_SERVER_INTERFACE_H
#define TAMP_SERVER_INTERFACE_H

#include "server_logic.h"

/**
 * @file server_interface.h
 * @brief Сетевой интерфейс сервера.
 */

/**
 * @class ServerInterface
 * @brief Отвечает за работу с сокетами и обработку клиентов.
 */
class ServerInterface {
public:
    /**
     * @brief Конструктор.
     * @param port Порт для прослушивания.
     * @param logic Ссылка на серверную логику.
     */
    ServerInterface(int port, ServerLogic& logic);

    /**
     * @brief Деструктор.
     */
    ~ServerInterface();

    /**
     * @brief Запускает сервер.
     */
    void run();

private:
    int port;              ///< Порт сервера
    int serverSocket;      ///< Серверный сокет
    ServerLogic& logic;    ///< Ссылка на бизнес-логику

    /**
     * @brief Инициализация сокета.
     */
    void initSocket();

    /**
     * @brief Обработка клиента.
     * @param clientSocket Сокет клиента.
     */
    void handleClient(int clientSocket);
};

#endif