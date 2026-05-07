#ifndef SMTPCLIENT_H
#define SMTPCLIENT_H

#include <QString>

/**
 * @file smtpclient_mailru.h
 * @brief SMTP-клиент для отправки email через Mail.ru по протоколу SMTP/SSL (порт 465).
 *
 * @details Учётные данные отправителя читаются из файла <tt>email.txt</tt>,
 * расположенного рядом с исполняемым файлом (или на уровень выше, или в текущей директории).
 * Формат файла:
 * @code
 *   email = sender@mail.ru
 *   key   = app_password_here
 * @endcode
 * Строки, начинающиеся с '#', игнорируются как комментарии.
 */

/**
 * @class SmtpClient
 * @brief Статический класс-утилита для отправки транзакционных email через Mail.ru SMTP.
 *
 * @details Все методы статические — создавать экземпляр класса не нужно.
 * Соединение устанавливается с нуля для каждого вызова (implicit SSL, порт 465).
 * Используется QSslSocket из Qt Network.
 *
 * @note Отправка блокирует вызывающий поток на время SMTP-транзакции.
 */
class SmtpClient {
public:
    static const QString smtpHost; ///< SMTP-сервер Mail.ru: «smtp.mail.ru».
    static const int     smtpPort; ///< Порт implicit SSL: 465.

    /**
     * @brief Считывает адрес отправителя из ключа «email» в файле email.txt.
     * @return Email отправителя, или пустая строка если ключ не найден.
     */
    static QString getSender();

    /**
     * @brief Считывает app-пароль отправителя из ключа «key» в файле email.txt.
     * @return Пароль приложения, или пустая строка если ключ не найден.
     */
    static QString getSenderPassword();

    /**
     * @brief Отправляет письмо с кодом верификации.
     * @param toEmail Email получателя.
     * @param code    6-значный код подтверждения (plain text).
     * @return @c true при успешной отправке, @c false при любой SMTP-ошибке.
     */
    static bool sendVerificationCode(const QString& toEmail, const QString& code);

    /**
     * @brief Отправляет письмо с кодом сброса пароля и именем пользователя в теле.
     * @param toEmail Email получателя.
     * @param login   Логин пользователя (вставляется в приветствие письма).
     * @param code    6-значный код сброса пароля (plain text).
     * @return @c true при успешной отправке, @c false при любой SMTP-ошибке.
     */
    static bool sendPasswordResetCode(const QString& toEmail,
                                      const QString& login,
                                      const QString& code);
};

#endif
