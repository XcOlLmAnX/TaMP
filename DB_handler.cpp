#include "DB_handler.h"
#include <iostream>

using namespace std;

db_handler::db_handler(const string& DBpath) : db(nullptr)
{
int rc = sqlite3_open(DBpath.c_str(), &db);
    if (rc != SQLITE_OK) {
throw runtime_error("Can't open database");
    }
    Tables();
}


db_handler::~db_handler()
{
    if (db) {
        sqlite3_close(db);
    }
}


void db_handler::Tables()
{
    const char* sql =
        "CREATE TABLE IF NOT EXISTS users_test ("
    "  id       INTEGER PRIMARY KEY AUTOINCREMENT,"
    "  name     TEXT    NOT NULL,"
    "  login    TEXT    NOT NULL UNIQUE,"
    "  hash     TEXT    NOT NULL,"
    "  email    TEXT    NOT NULL"
    ");";

    sqlite3_exec(db, sql, nullptr, nullptr, nullptr);
}


bool db_handler::reg_user(const string &login, const string& name, string &hash, const string &email)
{
    const char* sql = "INSERT INTO users_test (login, name, hash, email) VALUES (?, ?, ?, ?);";
    sqlite3_stmt *stmt = nullptr;
    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, login.c_str(),    -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, name.c_str(),    -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, hash.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, email.c_str(),    -1, SQLITE_STATIC);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return (rc == SQLITE_DONE);
}


bool db_handler::auth_user(const string &login, const string &hash)
{
    const char* sql ="SELECT 1 FROM users_test WHERE login = ? AND hash = ?;";
    sqlite3_stmt *stmt = nullptr;
    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, login.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, hash.c_str(), -1, SQLITE_STATIC);

    bool found = (sqlite3_step(stmt) == SQLITE_ROW);
    sqlite3_finalize(stmt);
    return found;
}


std::string db_handler::hashPassword(const std::string& password)
{
    return md5(password);
}



