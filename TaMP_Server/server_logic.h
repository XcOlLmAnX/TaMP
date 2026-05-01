#pragma once

#include "protocol.h"
#include "db_handler.h"
#include <string>
#include <map>

using namespace std;

/**
 * @file server_logic.h
 * @brief Бизнес-логика обработки запросов сервера.
 */

/**
 * @class ServerLogic
 * @brief Обрабатывает запросы клиентов и взаимодействует с БД.
 */
class ServerLogic {
public:
    /**
     * @brief Конструктор.
     * @param db Ссылка на обработчик базы данных.
     */
    explicit ServerLogic(DbHandler& db);

    /**
     * @brief Обрабатывает запрос клиента.
     * @param req Запрос.
     * @return Ответ сервера.
     */
    string handleRequest(const Request& req);

private:
    DbHandler& db; ///< Ссылка на базу данных

    map<string, string> resetCodes; ///< Коды сброса пароля (email -> код)

    /**
     * @brief Авторизация пользователя.
     */
    string handleAuth(const vector<string>& args);

    /**
     * @brief Регистрация пользователя.
     */
    string handleReg(const vector<string>& args);

    /**
     * @brief Запрос на сброс пароля.
     */
    string handleReset(const vector<string>& args);

    /**
     * @brief Подтверждение кода.
     */
    string handleConfirm(const vector<string>& args);

    /**
     * @brief Установка нового пароля.
     */
    string handleNewpass(const vector<string>& args);
};