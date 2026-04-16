#include "server_logic.h"

using namespace std;

ServerLogic::ServerLogic(DbHandler& db) : db(db) {}

string ServerLogic::handleRequest(const Request& req) {
    switch (req.type) {
        case CommandType::AUTH: return handleAuth(req.args);
        case CommandType::REG:  return handleReg(req.args);
        default:                return "error&unknown_command\n";
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