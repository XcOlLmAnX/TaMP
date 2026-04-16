#include "protocol.h"

#include <sstream>


static vector<string> split(const string& s, char delim)
{
    vector<string> tokens;
    istringstream stream(s);
    string token;
    while (getline(stream, token, delim)) {
        tokens.push_back(token);
    }
    return tokens;
}


Request parseRequest(const string& raw) {
Request req;
req.type = CommandType::UNKNOWN;
// Убираем возможный \r\n в конце (telnet-клиенты добавляют)
string trimmed = raw;
while (!trimmed.empty() && (trimmed.back() == '\r' || trimmed.back() == '\n')) {
    trimmed.pop_back();
}

if (trimmed.empty()) {
    return req;
}

vector<string> parts = split(trimmed, '&');
if (parts.empty()) {
    return req;
}

const string& cmd = parts[0];

// Определяем тип команды и проверяем число аргументов
if (cmd == "auth") {
    // auth&login&password — 3 части
    if (parts.size() == 3) {
        req.type = CommandType::AUTH;
        req.args = { parts[1], parts[2] };
    }
} else if (cmd == "reg") {
    // reg&login&password&email — 4 части
    if (parts.size() == 5) {
        req.type = CommandType::REG;
        req.args = { parts[1], parts[2], parts[3], parts[4]};
    }
} else if (cmd == "stat") {
    // stat&login — 2 части
    if (parts.size() == 2) {
        req.type = CommandType::STAT;
        req.args = { parts[1] };
    }
} else if (cmd == "check") {
    // check&task_number&variant&answer — 4 части
    if (parts.size() == 4) {
        req.type = CommandType::CHECK;
        req.args = { parts[1], parts[2], parts[3] };
    }
}

return req;

}


string makeAuthOk(const string& login) {
return "auth+&" + login + "\n";
}

string makeAuthFail() {
return "auth-\n";
}

string makeRegOk(const string& login) {
return "reg+&" + login + "\n";
}

string makeRegFail() {
return "reg-\n";
}

string makeStat(const vector<int>& scores, int total) {
// Формат: stat&score1$score2$…$scoreN&total
string result = "stat&";
for (size_t i = 0; i < scores.size(); ++i) {
if (i > 0) result += '$';
result += to_string(scores[i]);
}
result += '&';
result += to_string(total);
result += '\n';
return result;
}

string makeCheckOk() {
return "check+\n";
}

string makeCheckFail() {
return "check-\n";
}