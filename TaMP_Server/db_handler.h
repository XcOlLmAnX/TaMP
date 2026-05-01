#ifndef TAMP_DB_HANDLER_H
#define TAMP_DB_HANDLER_H

#include <string>
#include <sqlite3.h>

using namespace std;

/**
 * @file db_handler.h
 * @brief Класс для работы с базой данных пользователей.
 */

/**
 * @class DbHandler
 * @brief Обработчик SQLite базы данных.
 */
class DbHandler {
public:
    /**
     * @brief Конструктор.
     * @param dbPath Путь к файлу базы данных.
     */
    explicit DbHandler(const string& dbPath);

    /**
     * @brief Деструктор.
     */
    ~DbHandler();

    /**
     * @brief Авторизация пользователя.
     * @param login Логин.
     * @param hash Хэш пароля.
     * @return Имя пользователя или пустая строка.
     */
    string authUser(const string& login, const string& hash);

    /**
     * @brief Регистрация пользователя.
     */
    bool registerUser(const string& login, const string& name, const string& hash, const string& email);

    /**
     * @brief Проверка существования email.
     */
    bool emailExists(const string& email);

    /**
     * @brief Обновление пароля.
     */
    bool updatePassword(const string& email, const string& hash);

    /**
     * @brief возвращает email по логину
     */
    string getEmailByLogin(const string& login);

private:
    sqlite3* db; ///< Указатель на базу данных

    /**
     * @brief Инициализация таблиц.
     */
    void initTables();
};

#endif