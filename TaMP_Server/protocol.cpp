#include "protocol.h"
#include <sstream>

/**
 * @file protocol.cpp
 * @brief Разбор запросов и формирование ответов сервера.
 */

using namespace std;

/**
 * @brief Разбивает строку на части по разделителю.
 * @param s Исходная строка.
 * @param delim Разделитель.
 * @return Вектор строк.
 */
static vector<string> split(const string& s, char delim) {
    vector<string> tokens;
    istringstream stream(s);
    string token;
    while (getline(stream, token, delim))
        tokens.push_back(token);
    return tokens;
}

/**
 * @brief Парсит входящий запрос клиента.
 * @param raw Сырая строка запроса.
 * @return Структура Request с типом и аргументами.
 */
Request parseRequest(const string& raw) {
    Request req;
    req.type = CommandType::UNKNOWN;

    string trimmed = raw;
    while (!trimmed.empty() && (trimmed.back() == '\r' || trimmed.back() == '\n'))
        trimmed.pop_back();

    if (trimmed.empty()) return req;

    vector<string> parts = split(trimmed, '&');
    if (parts.empty()) return req;

    const string& cmd = parts[0];

    if (cmd == "auth") {
        if (parts.size() == 3) {
            req.type = CommandType::AUTH;
            req.args = { parts[1], parts[2] };
        }
    } else if (cmd == "reg") {
        if (parts.size() == 5) {
            req.type = CommandType::REG;
            req.args = { parts[1], parts[2], parts[3], parts[4] };
        }
    } else if (cmd == "stat") {
        if (parts.size() == 2) {
            req.type = CommandType::STAT;
            req.args = { parts[1] };
        }
    } else if (cmd == "check") {
        if (parts.size() == 4) {
            req.type = CommandType::CHECK;
            req.args = { parts[1], parts[2], parts[3] };
        }
    } else if (cmd == "reset") {
        if (parts.size() == 2) {
            req.type = CommandType::RESET;
            req.args = { parts[1] };
        }
    } else if (cmd == "confirm") {
        if (parts.size() == 3) {
            req.type = CommandType::CONFIRM;
            req.args = { parts[1], parts[2] };
        }
    } else if (cmd == "newpass") {
        if (parts.size() == 3) {
            req.type = CommandType::NEWPASS;
            req.args = { parts[1], parts[2] };
        }
    }

    return req;
}

/**
 * @brief Формирует успешный ответ авторизации.
 * @param name Имя пользователя.
 * @return Строка ответа.
 */
string makeAuthOk(const string& name)    { return "auth+&"    + name + "\n"; }

/**
 * @brief Формирует ответ ошибки авторизации.
 */
string makeAuthFail()                    { return "auth-\n"; }

/**
 * @brief Формирует успешный ответ регистрации.
 * @param login Логин пользователя.
 */
string makeRegOk(const string& login)    { return "reg+&"     + login + "\n"; }

/**
 * @brief Формирует ответ ошибки регистрации.
 */
string makeRegFail()                     { return "reg-\n"; }

/**
 * @brief Успешный ответ проверки.
 */
string makeCheckOk()                     { return "check+\n"; }

/**
 * @brief Ошибка проверки.
 */
string makeCheckFail()                   { return "check-\n"; }

/**
 * @brief Успешный ответ сброса.
 */
string makeResetOk()                     { return "reset+\n"; }

/**
 * @brief Ошибка сброса.
 */
string makeResetFail()                   { return "reset-\n"; }

/**
 * @brief Успешное подтверждение.
 */
string makeConfirmOk()                   { return "confirm+\n"; }

/**
 * @brief Ошибка подтверждения.
 */
string makeConfirmFail()                 { return "confirm-\n"; }

/**
 * @brief Успешная установка нового пароля.
 */
string makeNewpassOk()                   { return "newpass+\n"; }

/**
 * @brief Ошибка установки нового пароля.
 */
string makeNewpassFail()                 { return "newpass-\n"; }