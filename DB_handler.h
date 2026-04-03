#ifndef TESTTAMP_DB_HANDLER_H
#define TESTTAMP_DB_HANDLER_H

#include "md5_util.h"
#include <sqlite3.h>
#include <string>

using namespace std;

class db_handler {
public:
    explicit db_handler(const string& DBpath);
    ~db_handler();

    bool reg_user(const string& login, string& hash, const string& email);
    bool auth_user(const string& login, const string& hash);

    static std::string hashPassword(const std::string& password);

private:
    sqlite3 *db;

    void Tables();
};


#endif //TESTTAMP_DB_HANDLER_H