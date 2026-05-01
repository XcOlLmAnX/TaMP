#include "server_logic.h"
#include "smtpclient_mailru.h"
#include <iostream>
#include <random>
#include <QString>

using namespace std;

/**
 * @file server_logic.cpp
 * @brief Реализация бизнес-логики сервера.
 */

/**
 * @brief Конструктор.
 * @param db Ссылка на обработчик базы данных.
 */
ServerLogic::ServerLogic(DbHandler& db) : db(db) {}

/**
 * @brief Обрабатывает входящий запрос.
 * @param req Запрос клиента.
 * @return Строка ответа.
 */
string ServerLogic::handleRequest(const Request& req) {
    switch (req.type) {
        case CommandType::AUTH:    return handleAuth(req.args);
        case CommandType::REG:     return handleReg(req.args);
        case CommandType::RESET:   return handleReset(req.args);
        case CommandType::CONFIRM: return handleConfirm(req.args);
        case CommandType::NEWPASS: return handleNewpass(req.args);
        default:                   return "error&unknown_command\n";
    }
}

/**
 * @brief Обработка авторизации.
 */
string ServerLogic::handleAuth(const vector<string>& args) {
    if (args.size() < 2) return makeAuthFail();
    string name = db.authUser(args[0], args[1]);
    return name.empty() ? makeAuthFail() : makeAuthOk(name);
}

/**
 * @brief Обработка регистрации.
 */
string ServerLogic::handleReg(const vector<string>& args) {
    if (args.size() < 4) return makeRegFail();
    bool ok = db.registerUser(args[0], args[1], args[2], args[3]);
    return ok ? makeRegOk(args[0]) : makeRegFail();
}

/**
 * @brief Обработка запроса на сброс пароля.
 * Генерирует код и отправляет его на email.
 */
string ServerLogic::handleReset(const vector<string>& args) {
    if (args.size() < 1) return makeResetFail();

    const string& login = args[0];

    // Ищем email по логину
    string email = db.getEmailByLogin(login);
    if (email.empty()) return makeResetFail();

    // Генерируем код
    mt19937 rng(random_device{}());
    uniform_int_distribution<int> dist(100000, 999999);
    string code = to_string(dist(rng));

    // Сохраняем код по email
    resetCodes[login] = code;

    // Маскируем email: все символы до @ кроме последних 2 заменяем на *
    QString qEmail = QString::fromStdString(email);
    int atPos = qEmail.indexOf('@');
    QString masked = qEmail;
    if (atPos > 2) {
        masked = QString("*****") + qEmail.mid(atPos - 2);
    }

    // Отправляем письмо
    bool sent = SmtpClient::sendPasswordResetCode(
        QString::fromStdString(email),
        QString::fromStdString(login),
        QString::fromStdString(code)
    );

    if (!sent) return makeResetFail();

    // Возвращаем замаскированный email клиенту
    return "reset+&" + masked.toStdString() + "\n";
}

/**
 * @brief Подтверждение кода сброса пароля.
 */
string ServerLogic::handleConfirm(const vector<string>& args) {
    if (args.size() < 2) return makeConfirmFail();

    const string& login = args[0];
    const string& code  = args[1];

    auto it = resetCodes.find(login);
    if (it == resetCodes.end() || it->second != code)
        return makeConfirmFail();

    return makeConfirmOk();
}

/**
 * @brief Установка нового пароля.
 */
string ServerLogic::handleNewpass(const vector<string>& args) {
    if (args.size() < 2) return makeNewpassFail();

    const string& login = args[0];
    const string& hash  = args[1];

    resetCodes.erase(login);

    string email = db.getEmailByLogin(login);

    if (email.empty()) return makeNewpassFail();

    bool ok = db.updatePassword(email, hash);
    return ok ? makeNewpassOk() : makeNewpassFail();
}