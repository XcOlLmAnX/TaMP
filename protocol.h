#ifndef TAMP_PROTOCOL_H
#define TAMP_PROTOCOL_H

#pragma once

#include <string>
#include <vector>

using namespace std;


enum class CommandType
{
AUTH,    ///< Авторизация: auth&login&password
REG,     ///< Регистрация: reg&login&password&email
STAT,    ///< Статистика: stat&login
CHECK,   ///< Проверка ответа: check&task_number&variant&answer
UNKNOWN  ///< Неизвестная команда
};


struct Request
{
  CommandType type;      ///< Тип команды
  vector<string> args;   ///< Аргументы команды (без первого поля-команды)
};

/**

- @brief Разбирает строку запроса в структуру Request.
- @param raw Сырая строка, полученная от клиента.
- @return Заполненная структура Request. При ошибке type == CommandType::UNKNOWN.
  */
  Request parseRequest(const string& raw);

/**

- @brief Формирует строку успешного ответа на авторизацию.
- @param login Логин пользователя.
- @return Строка вида “auth+&login”.
  */
  string makeAuthOk(const string& login);

/**

- @brief Формирует строку неуспешного ответа на авторизацию.
- @return Строка “auth-”.
  */
  string makeAuthFail();

/**

- @brief Формирует строку успешного ответа на регистрацию.
- @param login Логин пользователя.
- @return Строка вида “reg+&login”.
  */
  string makeRegOk(const string& login);

/**

- @brief Формирует строку неуспешного ответа на регистрацию.
- @return Строка “reg-”.
  */
  string makeRegFail();

/**

- @brief Формирует строку ответа со статистикой.
-
- Формат: “stat&score1$score2$…$scoreN&total”
- где каждый score — результат по задаче, total — общая сумма.
-
- @param scores Вектор результатов по задачам.
- @param total  Общая сумма баллов.
- @return Строка вида “stat&3$6&21”.
  */
  string makeStat(const vector<int>& scores, int total);

/**

- @brief Формирует строку успешного ответа на проверку задачи.
- @return Строка “check+”.
  */
  string makeCheckOk();

/**

- @brief Формирует строку неуспешного ответа на проверку задачи.
- @return Строка “check-”.
  */
  string makeCheckFail();

#endif //TAMP_PROTOCOL_H