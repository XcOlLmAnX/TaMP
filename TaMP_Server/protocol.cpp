#include "protocol.h"
#include <sstream>

/**
 * @file protocol.cpp
 * @brief Реализация парсера запросов и функций формирования ответов протокола TaMP.
 */

using namespace std;

/**
 * @brief Разбивает строку на токены по заданному разделителю.
 * @param s     Исходная строка.
 * @param delim Символ-разделитель.
 * @return Вектор подстрок между вхождениями разделителя.
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
 * @brief Разбирает сырую строку запроса в структуру Request.
 *
 * @details Обрезает завершающие '\\r'/'\\n', делит строку по '&', сопоставляет
 * первый токен с известными командами и проверяет количество аргументов.
 * Несоответствие числа аргументов оставляет type == UNKNOWN.
 *
 * @param raw Сырая строка от клиента.
 * @return Структура Request с заполненными type и args.
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

/// @cond
string makeAuthOk(const string& name)         { return "auth+&"    + name        + "\n"; }
string makeAuthFail()                          { return "auth-\n"; }
string makeRegOk(const string& login)          { return "reg+&"     + login       + "\n"; }
string makeRegFail()                           { return "reg-\n"; }
string makeCheckOk()                           { return "check+\n"; }
string makeCheckFail()                         { return "check-\n"; }
string makeResetOk(const string& maskedEmail)  { return "reset+&"   + maskedEmail + "\n"; }
string makeResetFail()                         { return "reset-\n"; }
string makeConfirmOk()                         { return "confirm+\n"; }
string makeConfirmFail()                       { return "confirm-\n"; }
string makeNewpassOk()                         { return "newpass+\n"; }
string makeNewpassFail()                       { return "newpass-\n"; }
/// @endcond
