#ifndef SMTPCLIENT_H
#define SMTPCLIENT_H

#include <QString>

/**
 * @file smtpclient_mailru.h
 * @brief SMTP-клиент для отправки email через Mail.ru.
 */

/**
 * @class SmtpClient
 * @brief Класс для отправки email-сообщений (SMTP over SSL).
 */
class SmtpClient {
public:
    static const QString smtpHost; ///< SMTP сервер
    static const int     smtpPort; ///< SMTP порт

    /**
     * @brief Получает email отправителя из конфигурации.
     */
    static QString getSender();

    /**
     * @brief Получает пароль (ключ) отправителя.
     */
    static QString getSenderPassword();

    /**
     * @brief Отправляет код подтверждения.
     * @param toEmail Email получателя.
     * @param code Код подтверждения.
     * @return true, если письмо отправлено успешно.
     */
    static bool sendVerificationCode(const QString& toEmail, const QString& code);

    /**
     * @brief Отправляет код сброса пароля.
     * @param toEmail Email получателя.
     * @param login Логин пользователя.
     * @param code Код восстановления.
     * @return true, если письмо отправлено успешно.
     */
    static bool sendPasswordResetCode(const QString& toEmail,
                                      const QString& login,
                                      const QString& code);
};

#endif