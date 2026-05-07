#include "smtpclient_mailru.h"

#include <QSslSocket>
#include <QByteArray>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QCoreApplication>
#include <QDir>

/**
 * @file smtpclient_mailru.cpp
 * @brief Реализация SMTP/SSL-клиента для отправки писем через Mail.ru.
 */

const QString SmtpClient::smtpHost = "smtp.mail.ru";
const int     SmtpClient::smtpPort = 465;

/**
 * @brief Ищет файл email.txt по трём путям и возвращает значение по ключу.
 *
 * @details Пути поиска (в порядке приоритета):
 * 1. <applicationDir>/email.txt
 * 2. <applicationDir>/../email.txt
 * 3. <currentDir>/email.txt
 *
 * @param key Ключ для поиска в файле (например, "email" или "key").
 * @return Значение после символа '=' с обрезанными пробелами, или пустая строка.
 */
static QString readEmailTxtValue(const QString& key) {
    QStringList searchPaths = {
        QCoreApplication::applicationDirPath() + "/email.txt",
        QCoreApplication::applicationDirPath() + "/../email.txt",
        QDir::currentPath() + "/email.txt"
    };

    for (const QString& path : searchPaths) {
        QFile f(path);
        if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) continue;

        QTextStream in(&f);
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line.startsWith('#')) continue;

            int sep = line.indexOf('=');
            if (sep < 0) continue;

            QString k = line.left(sep).trimmed();
            QString v = line.mid(sep + 1).trimmed();

            if (k == key) {
                v.remove(' ');
                return v;
            }
        }
    }

    qWarning() << "[SmtpClient] email.txt: key" << key << "not found";
    return {};
}

/**
 * @brief Читает адрес отправителя из ключа «email» в email.txt.
 * @return Email отправителя или пустая строка.
 */
QString SmtpClient::getSender() {
    return readEmailTxtValue("email");
}

/**
 * @brief Читает app-пароль из ключа «key» в email.txt.
 * @return App-пароль отправителя или пустая строка.
 */
QString SmtpClient::getSenderPassword() {
    return readEmailTxtValue("key");
}

/**
 * @brief Ожидает поступления данных в сокет и читает всё доступное.
 *
 * @param socket    SSL-сокет для чтения.
 * @param timeoutMs Таймаут ожидания первых данных в миллисекундах (по умолчанию 15 с).
 * @return Все прочитанные байты; может быть пустым при таймауте.
 */
static QByteArray waitForData(QSslSocket* socket, int timeoutMs = 15000) {
    QByteArray result;
    if (socket->bytesAvailable() == 0)
        socket->waitForReadyRead(timeoutMs);
    result = socket->readAll();
    while (socket->waitForReadyRead(500))
        result.append(socket->readAll());
    return result;
}

/**
 * @brief Читает ответ SMTP-сервера и возвращает последнюю строку.
 *
 * @details Многострочные ответы SMTP (например, после EHLO) усекаются до
 * последней строки, которая содержит финальный код ответа.
 *
 * @param socket    SSL-сокет для чтения.
 * @param timeoutMs Таймаут ожидания в миллисекундах (по умолчанию 15 с).
 * @return Последняя строка ответа сервера, или пустая строка при таймауте.
 */
static QString readResponse(QSslSocket* socket, int timeoutMs = 15000) {
    QByteArray data = waitForData(socket, timeoutMs);
    QString all = QString::fromUtf8(data).trimmed();
    if (!all.isEmpty()) {
        QStringList lines = all.split('\n');
        QString last = lines.last().trimmed();
        qDebug() << "[SMTP] <--" << last;
        return last;
    }
    return {};
}

/**
 * @brief Отправляет SMTP-команду и возвращает ответ сервера.
 *
 * @param socket SSL-сокет для отправки.
 * @param cmd    Команда в формате SMTP, заканчивающаяся на <tt>\\r\\n</tt>.
 * @return Последняя строка ответа SMTP-сервера.
 */
static QString sendCmd(QSslSocket* socket, const QByteArray& cmd) {
    qDebug() << "[SMTP] -->" << cmd.trimmed();
    socket->write(cmd);
    socket->flush();
    socket->waitForBytesWritten(5000);
    return readResponse(socket);
}

/**
 * @brief Выполняет полный цикл SMTP-аутентификации и отправки одного письма.
 *
 * @details Последовательность команд: SSL-handshake → EHLO → AUTH LOGIN
 * → base64(login) → base64(password) → MAIL FROM → RCPT TO → DATA → QUIT.
 * Тема письма кодируется в Base64 UTF-8 (RFC 2047, формат =?UTF-8?B?...?=).
 *
 * @param toEmail     Email получателя.
 * @param subjectText Тема письма в UTF-8 (будет закодирована в Base64).
 * @param bodyText    Тело письма в UTF-8 (plain text).
 * @return @c true при получении кода 250 после DATA, @c false при любой ошибке.
 */
static bool doSend(const QString& toEmail,
                   const QString& subjectText,
                   const QString& bodyText) {
    QString senderEmail    = SmtpClient::getSender();
    QString senderPassword = SmtpClient::getSenderPassword();

    QSslSocket socket;
    socket.connectToHostEncrypted(SmtpClient::smtpHost, SmtpClient::smtpPort);
    socket.ignoreSslErrors();
    if (!socket.waitForEncrypted(20000)) {
        qDebug() << "[SMTP] SSL connection failed:" << socket.errorString();
        return false;
    }
    qDebug() << "[SMTP] SSL connected";

    QString resp = readResponse(&socket, 15000);
    if (resp.isEmpty() || !resp.startsWith("220")) return false;

    resp = sendCmd(&socket, "EHLO localhost\r\n");
    if (!resp.startsWith("250")) return false;

    resp = sendCmd(&socket, "AUTH LOGIN\r\n");
    if (!resp.startsWith("334")) return false;

    resp = sendCmd(&socket, senderEmail.toUtf8().toBase64() + "\r\n");
    if (!resp.startsWith("334")) return false;

    resp = sendCmd(&socket, senderPassword.toUtf8().toBase64() + "\r\n");
    if (!resp.startsWith("235")) return false;
    qDebug() << "[SMTP] Authenticated";

    resp = sendCmd(&socket, "MAIL FROM:<" + senderEmail.toUtf8() + ">\r\n");
    if (!resp.startsWith("250")) return false;

    resp = sendCmd(&socket, "RCPT TO:<" + toEmail.toUtf8() + ">\r\n");
    if (!resp.startsWith("250")) return false;

    resp = sendCmd(&socket, "DATA\r\n");
    if (!resp.startsWith("354")) return false;

    QByteArray subjectB64 = subjectText.toUtf8().toBase64();
    QByteArray email;
    email += "Subject: =?UTF-8?B?" + subjectB64 + "?=\r\n";
    email += "From: " + senderEmail.toUtf8() + "\r\n";
    email += "To: " + toEmail.toUtf8() + "\r\n";
    email += "Content-Type: text/plain; charset=UTF-8\r\n";
    email += "\r\n";
    email += bodyText.toUtf8() + "\r\n";
    email += ".\r\n";

    socket.write(email);
    socket.flush();
    socket.waitForBytesWritten(5000);
    resp = readResponse(&socket);
    if (!resp.startsWith("250")) return false;

    sendCmd(&socket, "QUIT\r\n");
    socket.disconnectFromHost();
    if (socket.state() != QAbstractSocket::UnconnectedState)
        socket.waitForDisconnected(5000);

    qDebug() << "[SMTP] Email sent to" << toEmail;
    return true;
}

/**
 * @brief Отправляет письмо с кодом верификации на указанный email.
 * @param toEmail Email получателя.
 * @param code    6-значный код.
 * @return @c true при успешной отправке.
 */
bool SmtpClient::sendVerificationCode(const QString& toEmail, const QString& code) {
    QString subject = QString::fromUtf8("Код подтверждения TaMP");
    QString body    = QString::fromUtf8("Ваш код подтверждения: ") + code;
    return doSend(toEmail, subject, body);
}

/**
 * @brief Отправляет письмо с кодом сброса пароля и приветствием по имени пользователя.
 * @param toEmail Email получателя.
 * @param login   Логин пользователя для приветствия в теле письма.
 * @param code    6-значный код сброса.
 * @return @c true при успешной отправке.
 */
bool SmtpClient::sendPasswordResetCode(const QString& toEmail,
                                      const QString& login,
                                      const QString& code) {
    QString subject = QString::fromUtf8("Восстановление пароля TaMP");
    QString body    =
        QString::fromUtf8("Здравствуйте, ") + login +
        QString::fromUtf8(".\n\nВаш код для сброса пароля: ") + code +
        QString::fromUtf8("\n\nЕсли вы не запрашивали сброс пароля - игнорируйте это письмо.");

    return doSend(toEmail, subject, body);
}
