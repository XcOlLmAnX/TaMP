#ifndef TAMP_PROTOCOL_H
#define TAMP_PROTOCOL_H

#include <string>
#include <vector>

/**
 * @file protocol.h
 * @brief Описание текстового протокола TaMP и вспомогательные функции формирования ответов.
 *
 * @details Протокол основан на текстовых строках в кодировке UTF-8, разделённых '\\n'.
 * Каждый запрос имеет вид: <tt>команда&аргумент1&аргумент2…\n</tt>.
 * Каждый ответ — <tt>команда±[&данные]\n</tt>, где '+' означает успех, '-' — ошибку.
 */

/**
 * @enum CommandType
 * @brief Типы команд, поддерживаемых сервером TaMP.
 */
enum class CommandType {
    AUTH,     ///< Авторизация: <tt>auth&<login>&<md5hash></tt>.
    REG,      ///< Регистрация: <tt>reg&<login>&<name>&<md5hash>&<email></tt>.
    STAT,     ///< Получение статистики (зарезервировано, не реализовано).
    CHECK,    ///< Проверка данных (зарезервировано, не реализовано).
    RESET,    ///< Запрос кода сброса пароля: <tt>reset&<login></tt>.
    CONFIRM,  ///< Подтверждение кода: <tt>confirm&<login>&<code></tt>.
    NEWPASS,  ///< Установка нового пароля: <tt>newpass&<login>&<md5hash></tt>.
    UNKNOWN   ///< Неизвестная или некорректно сформированная команда.
};

/**
 * @struct Request
 * @brief Разобранный запрос клиента.
 */
struct Request {
    CommandType              type; ///< Тип команды из перечисления CommandType.
    std::vector<std::string> args; ///< Аргументы команды (без первого токена-команды).
};

/**
 * @brief Разбирает сырую строку клиентского запроса в структуру Request.
 *
 * @details Разделитель аргументов — символ '&'. Завершающие '\\r' и '\\n' обрезаются.
 * Если команда неизвестна или количество аргументов не совпадает с ожидаемым,
 * возвращается Request с type == CommandType::UNKNOWN.
 *
 * @param raw Сырая строка запроса, полученная от клиента.
 * @return Заполненная структура Request.
 */
Request parseRequest(const std::string& raw);

/**
 * @brief Формирует ответ об успешной авторизации.
 * @param name Отображаемое имя пользователя.
 * @return Строка вида <tt>auth+&<name>\n</tt>.
 */
std::string makeAuthOk(const std::string& name);

/**
 * @brief Формирует ответ об ошибке авторизации (неверный логин или пароль).
 * @return Строка <tt>auth-\n</tt>.
 */
std::string makeAuthFail();

/**
 * @brief Формирует ответ об успешной регистрации.
 * @param login Логин зарегистрированного пользователя.
 * @return Строка вида <tt>reg+&<login>\n</tt>.
 */
std::string makeRegOk(const std::string& login);

/**
 * @brief Формирует ответ об ошибке регистрации (логин занят).
 * @return Строка <tt>reg-\n</tt>.
 */
std::string makeRegFail();

/**
 * @brief Формирует ответ об успешном результате команды CHECK.
 * @return Строка <tt>check+\n</tt>.
 */
std::string makeCheckOk();

/**
 * @brief Формирует ответ об ошибке команды CHECK.
 * @return Строка <tt>check-\n</tt>.
 */
std::string makeCheckFail();

/**
 * @brief Формирует ответ об успешной отправке кода сброса пароля.
 * @param maskedEmail Email с замаскированными символами (например, <tt>***** st@mail.ru</tt>).
 * @return Строка вида <tt>reset+&<maskedEmail>\n</tt>.
 */
std::string makeResetOk(const std::string& maskedEmail);

/**
 * @brief Формирует ответ об ошибке сброса пароля (логин не найден или ошибка отправки).
 * @return Строка <tt>reset-\n</tt>.
 */
std::string makeResetFail();

/**
 * @brief Формирует ответ об успешном подтверждении кода.
 * @return Строка <tt>confirm+\n</tt>.
 */
std::string makeConfirmOk();

/**
 * @brief Формирует ответ об ошибке подтверждения кода (неверный или просроченный код).
 * @return Строка <tt>confirm-\n</tt>.
 */
std::string makeConfirmFail();

/**
 * @brief Формирует ответ об успешной установке нового пароля.
 * @return Строка <tt>newpass+\n</tt>.
 */
std::string makeNewpassOk();

/**
 * @brief Формирует ответ об ошибке установки нового пароля.
 * @return Строка <tt>newpass-\n</tt>.
 */
std::string makeNewpassFail();

#endif
