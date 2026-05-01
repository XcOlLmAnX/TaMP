#include "protocol.h"
#include "db_handler.h"
#include "server_logic.h"
#include "server_interface.h"
#include <QCoreApplication>
#include <iostream>

using namespace std;

/**
 * @file main.cpp
 * @brief Точка входа в серверное приложение.
 */

/**
 * @brief Главная функция программы.
 * Инициализирует сервер и запускает его.
 * @param argc Количество аргументов командной строки.
 * @param argv Массив аргументов командной строки.
 * @return Код завершения программы.
 */
int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);

    try {
        DbHandler       db("server.db");        ///< Работа с базой данных
        ServerLogic     logic(db);                     ///< Бизнес-логика сервера
        ServerInterface server(12345, logic);      ///< Сервер (порт 12345)

        server.run();                                     ///< Запуск сервера
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}