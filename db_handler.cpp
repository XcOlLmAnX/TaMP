#include "db_handler.h"
#include <stdexcept>
#include <iostream>

using namespace std;

DbHandler::DbHandler(const string& dbPath) : db(nullptr) {
    int rc = sqlite3_open(dbPath.c_str(), &db);
    if (rc != SQLITE_OK)
        throw runtime_error("Cannot open database: " + string(sqlite3_errmsg(db)));
    initTables();
}

DbHandler::~DbHandler() {
    if (db) sqlite3_close(db);
}

void DbHandler::initTables() {
    const char* sql =
        "CREATE TABLE IF NOT EXISTS users ("
        "  id       INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  name     TEXT    NOT NULL,"
        "  login    TEXT    NOT NULL UNIQUE,"
        "  password TEXT    NOT NULL,"
        "  email    TEXT    NOT NULL"
        ");";

    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        string err = errMsg;
        sqlite3_free(errMsg);
        throw runtime_error("initTables failed: " + err);
    }
}

string DbHandler::authUser(const string& login, const string& hash) {
    const char* sql = "SELECT name FROM users WHERE login = ? AND password = ?;";
    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, login.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, hash.c_str(),  -1, SQLITE_STATIC);

    string name = "";
    if (sqlite3_step(stmt) == SQLITE_ROW)
        name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));

    sqlite3_finalize(stmt);
    return name;
}

bool DbHandler::registerUser(const string& login, const string& name,
                              const string& hash, const string& email) {
    const char* sql =
        "INSERT INTO users (login, name, password, email) VALUES (?, ?, ?, ?);";
    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, login.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, name.c_str(),  -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, hash.c_str(),  -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, email.c_str(), -1, SQLITE_STATIC);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE);
}