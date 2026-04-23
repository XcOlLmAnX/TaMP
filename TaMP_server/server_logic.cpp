#include "server_logic.h"
#include "smtpclient_mailru.h"
#include <random>
#include <QString>

using namespace std;

ServerLogic::ServerLogic(DbHandler& db) : db(db) {}

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

string ServerLogic::handleAuth(const vector<string>& args) {
    if (args.size() < 2) return makeAuthFail();
    string name = db.authUser(args[0], args[1]);
    return name.empty() ? makeAuthFail() : makeAuthOk(name);
}

string ServerLogic::handleReg(const vector<string>& args) {
    if (args.size() < 4) return makeRegFail();
    bool ok = db.registerUser(args[0], args[1], args[2], args[3]);
    return ok ? makeRegOk(args[0]) : makeRegFail();
}

string ServerLogic::handleReset(const vector<string>& args) {
    if (args.size() < 1) return makeResetFail();

    const string& email = args[0];

    if (!db.emailExists(email)) return makeResetFail();

    mt19937 rng(random_device{}());
    uniform_int_distribution<int> dist(100000, 999999);
    string code = to_string(dist(rng));

    resetCodes[email] = code;

    bool sent = SmtpClient::sendPasswordResetCode(
        QString::fromStdString(email),
        QString::fromStdString(email),
        QString::fromStdString(code)
    );

    return sent ? makeResetOk() : makeResetFail();
}

string ServerLogic::handleConfirm(const vector<string>& args) {
    if (args.size() < 2) return makeConfirmFail();

    const string& email = args[0];
    const string& code  = args[1];

    auto it = resetCodes.find(email);
    if (it == resetCodes.end() || it->second != code)
        return makeConfirmFail();

    return makeConfirmOk();
}

string ServerLogic::handleNewpass(const vector<string>& args) {
    if (args.size() < 2) return makeNewpassFail();

    const string& email = args[0];
    const string& hash  = args[1];

    resetCodes.erase(email);

    bool ok = db.updatePassword(email, hash);
    return ok ? makeNewpassOk() : makeNewpassFail();
}