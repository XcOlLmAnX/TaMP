#ifndef TAMP_PROTOCOL_H
#define TAMP_PROTOCOL_H

#include <string>
#include <vector>

using namespace std;

enum class CommandType {
  AUTH,
  REG,
  STAT,
  CHECK,
  RESET,
  CONFIRM,
  NEWPASS,
  UNKNOWN
};

struct Request {
  CommandType    type;
  vector<string> args;
};

Request parseRequest(const string& raw);

string makeAuthOk(const string& name);
string makeAuthFail();
string makeRegOk(const string& login);
string makeRegFail();
string makeStat(const vector<int>& scores, int total);
string makeCheckOk();
string makeCheckFail();
string makeResetOk();
string makeResetFail();
string makeConfirmOk();
string makeConfirmFail();
string makeNewpassOk();
string makeNewpassFail();

#endif