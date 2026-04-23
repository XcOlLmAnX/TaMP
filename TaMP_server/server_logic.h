#include "protocol.h"
#include "db_handler.h"
#include <string>
#include <map>

#pragma once


using namespace std;

class ServerLogic {
public:
    explicit ServerLogic(DbHandler& db);
    string handleRequest(const Request& req);

private:
    DbHandler& db;

    map<string, string> resetCodes;

    string handleAuth(const vector<string>& args);
    string handleReg(const vector<string>& args);
    string handleReset(const vector<string>& args);
    string handleConfirm(const vector<string>& args);
    string handleNewpass(const vector<string>& args);
};