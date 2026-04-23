#ifndef SMTPCLIENT_H
#define SMTPCLIENT_H

#include <QString>

class SmtpClient {
public:
    static const QString smtpHost;
    static const int     smtpPort;

    static QString getSender();
    static QString getSenderPassword();

    static bool sendVerificationCode(const QString& toEmail, const QString& code);
    static bool sendPasswordResetCode(const QString& toEmail, const QString& login, const QString& code);
};

#endif