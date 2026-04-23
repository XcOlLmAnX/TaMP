#ifndef TAMP_DB_HANDLER_H
#define TAMP_DB_HANDLER_H

#include <string>
#include <sqlite3.h>

using namespace std;

class DbHandler {
public:
    explicit DbHandler(const string& dbPath);
    ~DbHandler();

    string authUser(const string& login, const string& hash);

    bool registerUser(const string& login, const string& name, const string& hash, const string& email);

    bool emailExists(const string& email);
    bool updatePassword(const string& email, const string& hash);

private:
    sqlite3* db;
    void initTables();
};

#endif