#include <iostream>
#include "DB_handler.h"

using namespace  std;

int main() {
    db_handler db("users_test.db");
    cout << "DB opened" << endl;

    string hash = db_handler::hashPassword("password");
    cout << "Hash: " << hash << endl;

    bool reg = db.reg_user("nikolay", hash, "nikolay@mail.ru");
    cout << "reg: " << (reg ? "OK" : "FAIL") << endl;

    // При логине — тоже хешируем введённый пароль
    string authHash = db_handler::hashPassword("password");
    bool auth = db.auth_user("nikolay", authHash);
    cout << "auth: " << (auth ? "OK" : "FAIL") << endl;
}