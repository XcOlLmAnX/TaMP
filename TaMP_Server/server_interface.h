#ifndef TAMP_SERVER_INTERFACE_H
#define TAMP_SERVER_INTERFACE_H

#include <winsock2.h>
#include "server_logic.h"

/**
 * @file server_interface.h
 * @brief TCP-сервер на базе Winsock2 для приёма и обработки клиентов TaMP.
 */

/**
 * @class ServerInterface
 * @brief Управляет жизненным циклом серверного TCP-сокета и последовательной обработкой клиентов.
 *
 * @details Инициализирует Winsock2, создаёт, привязывает и переводит сокет в
 * режим прослушивания. Метод run() блокирует вызывающий поток в бесконечном цикле,
 * принимая по одному клиенту за раз (без многопоточности).
 *
 * @note Однопоточная архитектура: пока один клиент обслуживается, следующий ждёт в очереди.
 *
 * @throws std::runtime_error при ошибках Winsock (WSAStartup, socket, bind, listen).
 *
 * @see ServerLogic
 */
class ServerInterface {
public:
    /**
     * @brief Инициализирует Winsock2, создаёт и настраивает серверный сокет.
     * @param port  Порт для прослушивания (например, 12345).
     * @param logic Ссылка на объект бизнес-логики для обработки запросов.
     * @throws std::runtime_error при ошибке WSAStartup, socket(), bind() или listen().
     */
    ServerInterface(int port, ServerLogic& logic);

    /**
     * @brief Закрывает серверный сокет и завершает работу Winsock2 (WSACleanup).
     */
    ~ServerInterface();

    /**
     * @brief Запускает бесконечный цикл accept() → handleClient() → accept() …
     *
     * @details Для каждого принятого соединения выводит IP клиента в stdout,
     * обрабатывает все его запросы синхронно, затем закрывает сокет.
     * Ошибки accept() пропускаются с выводом в stderr.
     */
    void run();

private:
    int          port;         ///< Порт сервера, заданный в конструкторе.
    SOCKET       serverSocket; ///< Дескриптор прослушивающего сокета.
    ServerLogic& logic;        ///< Ссылка на обработчик бизнес-логики.

    /**
     * @brief Создаёт TCP-сокет, устанавливает SO_REUSEADDR, выполняет bind() и listen().
     * @throws std::runtime_error если любой из системных вызовов завершился с ошибкой.
     */
    void initSocket();

    /**
     * @brief Читает запросы клиента в цикле и отправляет ответы до разрыва соединения.
     *
     * @details Буфер recv составляет 1023 байта. Каждая принятая строка передаётся
     * в parseRequest(), затем в ServerLogic::handleRequest(). Цикл завершается
     * при recv() <= 0 (клиент отключился).
     *
     * @param clientSocket Дескриптор сокета принятого клиента.
     */
    void handleClient(SOCKET clientSocket);
};

#endif
