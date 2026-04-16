#ifndef TAMP_DB_HANDLER_H
#define TAMP_DB_HANDLER_H

#pragma once

#include <string>
#include <sqlite3.h>

using namespace std;

class DbHandler {
public:
  explicit DbHandler(const string& dbPath);
  ~DbHandler();

  /// @brief Проверяет логин+хеш. Возвращает имя пользователя или "".
  string authUser(const string& login, const string& hash);

  /// @brief Регистрирует пользователя. false если логин занят.
  bool registerUser(const string& login, const string& name,
                    const string& hash, const string& email);

private:
  sqlite3* db;
  void initTables();
};

#endif