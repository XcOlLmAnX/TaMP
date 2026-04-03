#include <iostream>
#include "DB_handler.h"

using namespace  std;

int main()
{
    db_handler db("users_test.db");
    cout << "DB opened" << endl;

    string hash = db_handler::hashPassword("secret");
    cout << "Hash: " << hash << endl;

    bool reg = db.reg_user("begoode", "nicolay", hash, "nikolay@mail.ru");
    cout << "reg: " << (reg ? "OK" : "FAIL") << endl;

    string authHash = db_handler::hashPassword("secret");
    bool auth = db.auth_user("begoode", authHash);
    cout << "auth: " << (auth ? "OK" : "FAIL") << endl;
}