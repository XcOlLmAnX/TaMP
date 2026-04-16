#include "protocol.h"
#include "db_handler.h"
#include "server_logic.h"
#include "server_interface.h"
#include <iostream>

using namespace std;

/**
 * @file main.cpp
 * @brief Точка входа TCP сервера.
 */

int main() {
    try {
        DbHandler      db("server.db");
        ServerLogic    logic(db);
        ServerInterface server(12345, logic);

        server.run();
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}