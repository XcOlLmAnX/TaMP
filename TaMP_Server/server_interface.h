#ifndef TAMP_SERVER_INTERFACE_H
#define TAMP_SERVER_INTERFACE_H

#include <QTcpServer>
#include <QTcpSocket>
#include "server_logic.h"

/**
 * @file server_interface.h
 * @brief TCP-сервер на базе QTcpServer для приёма и обработки клиентов TaMP.
 */

/**
 * @class ServerInterface
 * @brief Управляет жизненным циклом серверного TCP-сокета и последовательной обработкой клиентов.
 *
 * @details Инициализирует QTcpServer, привязывает к порту и переводит в режим
 * прослушивания. Метод run() блокирует вызывающий поток в бесконечном цикле,
 * принимая по одному клиенту за раз (без многопоточности).
 *
 * @note Однопоточная архитектура: пока один клиент обслуживается, следующий ждёт в очереди.
 *
 * @throws std::runtime_error при ошибке listen().
 *
 * @see ServerLogic
 */
class ServerInterface {
public:
    /**
     * @brief Инициализирует QTcpServer и начинает прослушивание на указанном порту.
     * @param port  Порт для прослушивания (например, 12345).
     * @param logic Ссылка на объект бизнес-логики для обработки запросов.
     * @throws std::runtime_error при ошибке listen().
     */
    ServerInterface(int port, ServerLogic& logic);

    /**
     * @brief Закрывает серверный сокет.
     */
    ~ServerInterface();

    /**
     * @brief Запускает бесконечный цикл waitForNewConnection() → handleClient() → …
     *
     * @details Для каждого принятого соединения выводит IP клиента в stdout,
     * обрабатывает все его запросы синхронно, затем закрывает сокет.
     */
    void run();

private:
    QTcpServer*  server; ///< Qt TCP-сервер.
    ServerLogic& logic;  ///< Ссылка на обработчик бизнес-логики.

    /**
     * @brief Читает запросы клиента в цикле и отправляет ответы до разрыва соединения.
     * @param client Сокет принятого клиента.
     */
    void handleClient(QTcpSocket* client);
};

#endif
