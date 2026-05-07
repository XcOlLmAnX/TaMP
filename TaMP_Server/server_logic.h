#pragma once

#include "protocol.h"
#include "db_handler.h"
#include <string>
#include <map>

/**
 * @file server_logic.h
 * @brief Бизнес-логика обработки клиентских запросов сервера TaMP.
 */

/**
 * @class ServerLogic
 * @brief Диспетчер запросов: маршрутизирует команды к специализированным обработчикам.
 *
 * @details Принимает разобранный Request и возвращает строку ответа в формате
 * протокола TaMP. Взаимодействует с DbHandler для операций с базой данных
 * и со SmtpClient для отправки email при сбросе пароля.
 *
 * Временное хранилище кодов сброса (resetCodes) живёт в памяти процесса —
 * коды не персистентны и сбрасываются при перезапуске сервера.
 *
 * @see DbHandler, SmtpClient, ServerInterface
 */
class ServerLogic {
public:
    /**
     * @brief Создаёт объект бизнес-логики, привязанный к указанному DbHandler.
     * @param db Ссылка на инициализированный обработчик базы данных.
     */
    explicit ServerLogic(DbHandler& db);

    /**
     * @brief Маршрутизирует запрос к соответствующему обработчику и возвращает ответ.
     *
     * @details Неизвестные команды возвращают <tt>"error&unknown_command\n"</tt>.
     *
     * @param req Разобранный запрос клиента.
     * @return Строка ответа в формате протокола TaMP.
     */
    std::string handleRequest(const Request& req);

private:
    DbHandler& db;                              ///< Ссылка на обработчик базы данных.
    std::map<std::string, std::string> resetCodes; ///< Временные коды сброса: логин → код.

    /**
     * @brief Обрабатывает команду AUTH: проверяет логин и MD5-хэш пароля.
     * @param args Аргументы: {login, hash}. При неверном размере возвращает ошибку.
     * @return Строка ответа (makeAuthOk / makeAuthFail).
     */
    std::string handleAuth(const std::vector<std::string>& args);

    /**
     * @brief Обрабатывает команду REG: регистрирует нового пользователя.
     * @param args Аргументы: {login, name, hash, email}. При неверном размере возвращает ошибку.
     * @return Строка ответа (makeRegOk / makeRegFail).
     */
    std::string handleReg(const std::vector<std::string>& args);

    /**
     * @brief Обрабатывает команду RESET: генерирует код и отправляет его на email.
     *
     * @details Генерирует 6-значный случайный код (mt19937), сохраняет его в resetCodes
     * и отправляет через SmtpClient::sendPasswordResetCode(). Email маскируется перед
     * отправкой ответа клиенту.
     *
     * @param args Аргументы: {login}. При неверном размере или отсутствии логина — ошибка.
     * @return Строка ответа с замаскированным email (makeResetOk) или ошибка (makeResetFail).
     */
    std::string handleReset(const std::vector<std::string>& args);

    /**
     * @brief Обрабатывает команду CONFIRM: проверяет код сброса пароля.
     * @param args Аргументы: {login, code}.
     * @return makeConfirmOk() если код совпадает, иначе makeConfirmFail().
     */
    std::string handleConfirm(const std::vector<std::string>& args);

    /**
     * @brief Обрабатывает команду NEWPASS: удаляет код из resetCodes и обновляет пароль в БД.
     * @param args Аргументы: {login, hash}.
     * @return makeNewpassOk() при успехе, makeNewpassFail() при ошибке.
     */
    std::string handleNewpass(const std::vector<std::string>& args);
};
