#include "protocol.h"
#include "db_handler.h"
#include "server_logic.h"
#include "server_interface.h"
#include <QCoreApplication>
#include <iostream>

using namespace std;

/**
 * @file main.cpp
 * @brief Точка входа серверного приложения TaMP.
 *
 * @details Инициализирует три ключевых компонента:
 * - DbHandler — работа с SQLite-базой данных пользователей;
 * - ServerLogic — бизнес-логика обработки запросов;
 * - ServerInterface — TCP-сервер на базе Winsock2.
 *
 * При ошибке инициализации любого компонента в stderr выводится сообщение
 * и возвращается код завершения 1.
 */

/**
 * @brief Запускает серверное приложение TaMP.
 *
 * @details Создаёт QCoreApplication (необходим для Qt SQL и SSL), затем
 * последовательно инициализирует DbHandler, ServerLogic и ServerInterface.
 * ServerInterface::run() блокирует поток и обрабатывает клиентов последовательно
 * (один за другим без многопоточности).
 *
 * @param argc Количество аргументов командной строки.
 * @param argv Массив строк аргументов командной строки.
 * @return 0 при нормальном завершении, 1 при исключении инициализации.
 */
int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);

    try {
        DbHandler       db("users_new.db");
        ServerLogic     logic(db);
        ServerInterface server(12345, logic);

        server.run();
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}
