#ifndef TAMP_PROTOCOL_H
#define TAMP_PROTOCOL_H

#include <string>
#include <vector>

using namespace std;

/**
 * @file protocol.h
 * @brief Описание протокола взаимодействия клиента и сервера.
 */

/**
 * @enum CommandType
 * @brief Типы команд, поддерживаемые сервером.
 */
enum class CommandType {
  AUTH,     ///< Авторизация
  REG,      ///< Регистрация
  STAT,     ///< Получение статистики
  CHECK,    ///< Проверка данных
  RESET,    ///< Запрос на сброс
  CONFIRM,  ///< Подтверждение
  NEWPASS,  ///< Установка нового пароля
  UNKNOWN   ///< Неизвестная команда
};

/**
 * @struct Request
 * @brief Структура запроса клиента.
 */
struct Request {
  CommandType    type; ///< Тип команды
  vector<string> args; ///< Аргументы команды
};

/**
 * @brief Парсит строку запроса.
 * @param raw Сырая строка.
 * @return Структура Request.
 */
Request parseRequest(const string& raw);

/**
 * @brief Успешная авторизация.
 */
string makeAuthOk(const string& name);

/**
 * @brief Ошибка авторизации.
 */
string makeAuthFail();

/**
 * @brief Успешная регистрация.
 */
string makeRegOk(const string& login);

/**
 * @brief Ошибка регистрации.
 */
string makeRegFail();

/**
 * @brief Успешная проверка.
 */
string makeCheckOk();

/**
 * @brief Ошибка проверки.
 */
string makeCheckFail();

/**
 * @brief Успешный сброс.
 */
string makeResetOk();

/**
 * @brief Ошибка сброса.
 */
string makeResetFail();

/**
 * @brief Успешное подтверждение.
 */
string makeConfirmOk();

/**
 * @brief Ошибка подтверждения.
 */
string makeConfirmFail();

/**
 * @brief Успешная установка нового пароля.
 */
string makeNewpassOk();

/**
 * @brief Ошибка установки нового пароля.
 */
string makeNewpassFail();

#endif