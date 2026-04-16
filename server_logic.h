#pragma once

#include "protocol.h"
#include "db_handler.h"
#include <string>

using namespace std;

/**
 * @file server_logic.h
 * @brief Controller слой MVC — обработка запросов и формирование ответов.
 *
 * Связывает протокол (Request) с базой данных (DbHandler).
 * Не знает ничего о сети — только бизнес-логика.
 */

/**
 * @brief Класс контроллера сервера.
 *
 * Принимает разобранный запрос, обращается к БД и возвращает
 * готовую строку ответа по протоколу.
 */
class ServerLogic {
public:
    /**
     * @brief Конструктор. Принимает ссылку на DbHandler.
     * @param db Ссылка на объект работы с БД.
     */
    explicit ServerLogic(DbHandler& db);

    /**
     * @brief Обрабатывает запрос и возвращает строку ответа.
     * @param req Разобранный запрос от клиента.
     * @return Строка ответа по протоколу.
     */
    string handleRequest(const Request& req);

private:
    DbHandler& db;  ///< Ссылка на Model слой

    /**
     * @brief Обрабатывает авторизацию.
     * @param args Аргументы: [login, password].
     * @return "auth+&login" или "auth-".
     */
    string handleAuth(const vector<string>& args);

    /**
     * @brief Обрабатывает регистрацию.
     * @param args Аргументы: [login, password, email].
     * @return "reg+&login" или "reg-".
     */
    string handleReg(const vector<string>& args);

    /**
     * @brief Обрабатывает запрос статистики.
     * @param args Аргументы: [login].
     * @return "stat&score1$score2&total".
     */
    string handleStat(const vector<string>& args);

    /**
     * @brief Обрабатывает проверку ответа на задачу.
     * @param args Аргументы: [task_number, variant, answer].
     * @return "check+" или "check-".
     */
    string handleCheck(const vector<string>& args);
};
