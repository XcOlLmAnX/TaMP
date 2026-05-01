#include "db_handler.h"
#include <stdexcept>
#include <iostream>

using namespace std;
/**
 * @file db_handler.cpp
 * @brief Реализация класса DbHandler.
 */

/**
 * @brief Конструктор обработчика базы данных.
 * Открывает соединение с БД и инициализирует таблицы.
 * @param dbPath Путь к файлу базы данных.
 */
DbHandler::DbHandler(const string& dbPath) : db(nullptr) {
    int rc = sqlite3_open(dbPath.c_str(), &db);
    if (rc != SQLITE_OK)
        throw runtime_error("Cannot open database: " + string(sqlite3_errmsg(db)));
    initTables();
}

/**
 * @brief Деструктор.
 * Закрывает соединение с базой данных.
 */
DbHandler::~DbHandler() {
    if (db) sqlite3_close(db);
}

/**
 * @brief Создает таблицы, если они не существуют.
 */
void DbHandler::initTables() {
    const char* sql =
        "CREATE TABLE IF NOT EXISTS users ("
        "  id       INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  name     TEXT    NOT NULL,"
        "  login    TEXT    NOT NULL UNIQUE,"
        "  hash TEXT    NOT NULL,"
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

/**
 * @brief Проверяет пользователя по логину и хэшу пароля.
 * @param login Логин пользователя.
 * @param hash Хэш пароля.
 * @return Имя пользователя, если найден, иначе пустая строка.
 */
string DbHandler::authUser(const string& login, const string& hash) {
    const char* sql = "SELECT name FROM users WHERE login = ? AND hash = ?;";
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

/**
 * @brief Регистрирует нового пользователя.
 * @param login Логин.
 * @param name Имя пользователя.
 * @param hash Хэш пароля.
 * @param email Email пользователя.
 * @return true, если регистрация прошла успешно.
 */
bool DbHandler::registerUser(const string& login, const string& name,
                              const string& hash, const string& email) {
    const char* sql =
        "INSERT INTO users (login, name, hash, email) VALUES (?, ?, ?, ?);";
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

/**
 * @brief Проверяет, существует ли пользователь с таким email.
 * @param email Email для проверки.
 * @return true, если email найден.
 */
bool DbHandler::emailExists(const string& email) {
    const char* sql = "SELECT 1 FROM users WHERE email = ?;";
    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, email.c_str(), -1, SQLITE_STATIC);
    bool found = (sqlite3_step(stmt) == SQLITE_ROW);
    sqlite3_finalize(stmt);
    return found;
}

/**
 * @brief Обновляет пароль пользователя по email.
 * @param email Email пользователя.
 * @param hash Новый хэш пароля.
 * @return true, если обновление прошло успешно.
 */
bool DbHandler::updatePassword(const string& email, const string& hash) {
    const char* sql = "UPDATE users SET hash = ? WHERE email = ?;";
    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, hash.c_str(),  -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, email.c_str(), -1, SQLITE_STATIC);
    int rc = sqlite3_step(stmt);
    int rows = sqlite3_changes(db);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE && rows > 0);
}


string DbHandler::getEmailByLogin(const string& login) {
    const char* sql = "SELECT email FROM users WHERE login = ?;";
    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, login.c_str(), -1, SQLITE_STATIC);

    string email = "";
    if (sqlite3_step(stmt) == SQLITE_ROW)
        email = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));

    sqlite3_finalize(stmt);
    return email;
}