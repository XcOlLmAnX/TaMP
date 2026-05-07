#include "server_logic.h"
#include "smtpclient_mailru.h"
#include <iostream>
#include <random>
#include <QString>

using namespace std;

/**
 * @file server_logic.cpp
 * @brief Реализация бизнес-логики сервера TaMP.
 */

/**
 * @brief Инициализирует объект ссылкой на DbHandler.
 */
ServerLogic::ServerLogic(DbHandler& db) : db(db) {}

/**
 * @brief Диспетчеризирует запрос по типу команды.
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
 * @brief Проверяет учётные данные через DbHandler::authUser().
 */
string ServerLogic::handleAuth(const vector<string>& args) {
    if (args.size() < 2) return makeAuthFail();
    string name = db.authUser(args[0], args[1]);
    return name.empty() ? makeAuthFail() : makeAuthOk(name);
}

/**
 * @brief Регистрирует нового пользователя через DbHandler::registerUser().
 */
string ServerLogic::handleReg(const vector<string>& args) {
    if (args.size() < 4) return makeRegFail();
    bool ok = db.registerUser(args[0], args[1], args[2], args[3]);
    return ok ? makeRegOk(args[0]) : makeRegFail();
}

/**
 * @brief Ищет email по логину, генерирует 6-значный код и отправляет письмо.
 *
 * @details Код хранится в resetCodes[login] до подтверждения.
 * Email маскируется: все символы до '@' кроме последних двух заменяются на '*****'.
 *
 */
string ServerLogic::handleReset(const vector<string>& args) {
    if (args.size() < 1) return makeResetFail();

    const string& login = args[0];

    string email = db.getEmailByLogin(login);
    if (email.empty()) return makeResetFail();

    mt19937 rng(random_device{}());
    uniform_int_distribution<int> dist(100000, 999999);
    string code = to_string(dist(rng));

    resetCodes[login] = code;

    QString qEmail = QString::fromStdString(email);
    int atPos = qEmail.indexOf('@');
    QString masked = qEmail;
    if (atPos > 2) {
        masked = QString("*****") + qEmail.mid(atPos - 2);
    }

    bool sent = SmtpClient::sendPasswordResetCode(
        QString::fromStdString(email),
        QString::fromStdString(login),
        QString::fromStdString(code)
    );

    if (!sent) return makeResetFail();

    return makeResetOk(masked.toStdString());
}

/**
 * @brief Проверяет код из resetCodes против введённого пользователем.
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
 * @brief Удаляет код сброса и обновляет пароль в БД через DbHandler::updatePassword().
 *
 * @details Код удаляется из resetCodes независимо от результата обновления пароля,
 * чтобы предотвратить повторное использование.
 *
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
