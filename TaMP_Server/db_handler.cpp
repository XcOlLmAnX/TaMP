#include "db_handler.h"
#include <QSqlQuery>
#include <QSqlError>
#include <stdexcept>

/**
 * @file db_handler.cpp
 * @brief Реализация класса DbHandler: операции с SQLite через Qt SQL.
 */

/**
 * @brief Открывает БД по пути dbPath и вызывает initTables().
 * @throws std::runtime_error если db.open() вернул false.
 */
DbHandler::DbHandler(const std::string& dbPath) {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QString::fromStdString(dbPath));
    if (!db.open())
        throw std::runtime_error("Cannot open database: " + db.lastError().text().toStdString());
    initTables();
}

/**
 * @brief Закрывает соединение с БД.
 */
DbHandler::~DbHandler() {
    db.close();
}

/**
 * @brief Выполняет CREATE TABLE IF NOT EXISTS для таблицы users.
 * @throws std::runtime_error если DDL-запрос завершился с ошибкой.
 */
void DbHandler::initTables() {
    QSqlQuery q(db);
    if (!q.exec(
            "CREATE TABLE IF NOT EXISTS users ("
            "  id    INTEGER PRIMARY KEY AUTOINCREMENT,"
            "  name  TEXT    NOT NULL,"
            "  login TEXT    NOT NULL UNIQUE,"
            "  hash  TEXT    NOT NULL,"
            "  email TEXT    NOT NULL"
            ");"
        ))
        throw std::runtime_error("initTables failed: " + q.lastError().text().toStdString());
}

/**
 * @brief Ищет пользователя по логину и MD5-хэшу пароля.
 * @param login Логин.
 * @param hash  MD5-хэш пароля.
 * @return Поле name при совпадении, пустая строка иначе.
 */
std::string DbHandler::authUser(const std::string& login, const std::string& hash) {
    QSqlQuery q(db);
    q.prepare("SELECT name FROM users WHERE login = :login AND hash = :hash;");
    q.bindValue(":login", QString::fromStdString(login));
    q.bindValue(":hash",  QString::fromStdString(hash));
    q.exec();
    if (q.next())
        return q.value(0).toString().toStdString();
    return {};
}

/**
 * @brief Вставляет новую запись в таблицу users.
 * @param login Логин (UNIQUE).
 * @param name  Имя.
 * @param hash  MD5-хэш пароля.
 * @param email Email.
 * @return @c true при успехе, @c false при нарушении UNIQUE-ограничения (логин занят).
 */
bool DbHandler::registerUser(const std::string& login, const std::string& name,
                              const std::string& hash, const std::string& email) {
    QSqlQuery q(db);
    q.prepare("INSERT INTO users (login, name, hash, email) VALUES (:login, :name, :hash, :email);");
    q.bindValue(":login", QString::fromStdString(login));
    q.bindValue(":name",  QString::fromStdString(name));
    q.bindValue(":hash",  QString::fromStdString(hash));
    q.bindValue(":email", QString::fromStdString(email));
    return q.exec();
}

/**
 * @brief Проверяет наличие строки с указанным email в таблице users.
 * @param email Email для поиска.
 * @return @c true если запись найдена.
 */
bool DbHandler::emailExists(const std::string& email) {
    QSqlQuery q(db);
    q.prepare("SELECT 1 FROM users WHERE email = :email;");
    q.bindValue(":email", QString::fromStdString(email));
    q.exec();
    return q.next();
}

/**
 * @brief Обновляет поле hash у записи с указанным email.
 * @param email Email пользователя.
 * @param hash  Новый MD5-хэш пароля.
 * @return @c true если запрос выполнен и изменена хотя бы одна строка.
 */
bool DbHandler::updatePassword(const std::string& email, const std::string& hash) {
    QSqlQuery q(db);
    q.prepare("UPDATE users SET hash = :hash WHERE email = :email;");
    q.bindValue(":hash",  QString::fromStdString(hash));
    q.bindValue(":email", QString::fromStdString(email));
    return q.exec() && q.numRowsAffected() > 0;
}

/**
 * @brief Возвращает поле email по логину пользователя.
 * @param login Логин.
 * @return Строка email при нахождении, пустая строка иначе.
 */
std::string DbHandler::getEmailByLogin(const std::string& login) {
    QSqlQuery q(db);
    q.prepare("SELECT email FROM users WHERE login = :login;");
    q.bindValue(":login", QString::fromStdString(login));
    q.exec();
    if (q.next())
        return q.value(0).toString().toStdString();
    return {};
}
