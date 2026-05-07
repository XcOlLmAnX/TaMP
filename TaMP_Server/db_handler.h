#ifndef TAMP_DB_HANDLER_H
#define TAMP_DB_HANDLER_H

#include <string>
#include <QSqlDatabase>

/**
 * @file db_handler.h
 * @brief RAII-обёртка над Qt SQL (SQLite) для хранения данных пользователей TaMP.
 */

/**
 * @class DbHandler
 * @brief Управляет SQLite-базой данных пользователей через Qt SQL.
 *
 * @details Конструктор открывает БД и создаёт необходимые таблицы (если их нет).
 * Деструктор закрывает соединение. Все публичные методы работают синхронно
 * в вызывающем потоке.
 *
 * Схема таблицы <tt>users</tt>:
 * @code
 *   id    INTEGER PRIMARY KEY AUTOINCREMENT
 *   name  TEXT    NOT NULL
 *   login TEXT    NOT NULL UNIQUE
 *   hash  TEXT    NOT NULL       -- MD5-хэш пароля
 *   email TEXT    NOT NULL
 * @endcode
 *
 * @throws std::runtime_error при ошибке открытия БД или инициализации таблиц.
 *
 * @see ServerLogic
 */
class DbHandler {
public:
    /**
     * @brief Открывает SQLite-базу данных и инициализирует таблицы.
     * @param dbPath Путь к файлу БД (создаётся автоматически при отсутствии).
     * @throws std::runtime_error если БД не удалось открыть или создать таблицы.
     */
    explicit DbHandler(const std::string& dbPath);

    /**
     * @brief Закрывает соединение с базой данных.
     */
    ~DbHandler();

    /**
     * @brief Проверяет пару логин/хэш и возвращает имя пользователя.
     * @param login Логин пользователя.
     * @param hash  MD5-хэш введённого пароля.
     * @return Отображаемое имя при совпадении, пустая строка если не найден.
     */
    std::string authUser(const std::string& login, const std::string& hash);

    /**
     * @brief Добавляет новую запись пользователя в базу данных.
     * @param login Уникальный логин нового пользователя.
     * @param name  Отображаемое имя.
     * @param hash  MD5-хэш пароля.
     * @param email Адрес электронной почты.
     * @return @c true при успехе, @c false если логин уже занят (нарушение UNIQUE).
     */
    bool registerUser(const std::string& login, const std::string& name,
                      const std::string& hash,  const std::string& email);

    /**
     * @brief Проверяет, зарегистрирован ли указанный email в системе.
     * @param email Email для поиска.
     * @return @c true если запись с таким email существует.
     */
    bool emailExists(const std::string& email);

    /**
     * @brief Обновляет хэш пароля пользователя, найденного по email.
     * @param email Email пользователя.
     * @param hash  Новый MD5-хэш пароля.
     * @return @c true если запись найдена и обновлена.
     */
    bool updatePassword(const std::string& email, const std::string& hash);

    /**
     * @brief Возвращает email пользователя по его логину.
     * @param login Логин пользователя.
     * @return Email при нахождении, пустая строка если логин не существует.
     */
    std::string getEmailByLogin(const std::string& login);

private:
    QSqlDatabase db; ///< Дескриптор соединения с SQLite через Qt SQL.

    /**
     * @brief Создаёт таблицы БД, если они ещё не существуют.
     * @throws std::runtime_error при ошибке выполнения DDL-запроса.
     */
    void initTables();
};

#endif
