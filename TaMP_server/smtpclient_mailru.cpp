#include "smtpclient_mailru.h"

#include <QSslSocket>
#include <QByteArray>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QCoreApplication>
#include <QDir>

const QString SmtpClient::smtpHost = "smtp.mail.ru";

const int     SmtpClient::smtpPort = 465;

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

QString SmtpClient::getSender()    { return readEmailTxtValue("email"); }
QString SmtpClient::getSenderPassword() { return readEmailTxtValue("key");   }

static QByteArray waitForData(QSslSocket* socket, int timeoutMs = 15000) {
    QByteArray result;
    if (socket->bytesAvailable() == 0)
        socket->waitForReadyRead(timeoutMs);
    result = socket->readAll();
    while (socket->waitForReadyRead(500))
        result.append(socket->readAll());
    return result;
}

static QString readResponse(QSslSocket* socket, int timeoutMs = 15000) {
    QByteArray data = waitForData(socket, timeoutMs);
    QString all = QString::fromUtf8(data).trimmed();
    if (!all.isEmpty()) {
        QStringList lines = all.split('\n');
        QString last = lines.last().trimmed();
        qDebug() << "[SMTP] <--" << last;
        return last;
    }
    qDebug() << "[SMTP] readResponse: no data received";
    return {};
}

static QString sendCmd(QSslSocket* socket, const QByteArray& cmd) {
    qDebug() << "[SMTP] -->" << cmd.trimmed();
    socket->write(cmd);
    socket->flush();
    socket->waitForBytesWritten(5000);
    return readResponse(socket);
}

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
    if (!resp.startsWith("250")) {
        qDebug() << "[SMTP] Email rejected:" << resp;
        return false;
    }

    sendCmd(&socket, "QUIT\r\n");
    socket.disconnectFromHost();
    if (socket.state() != QAbstractSocket::UnconnectedState)
        socket.waitForDisconnected(5000);

    qDebug() << "[SMTP] Email sent to" << toEmail;
    return true;
}

bool SmtpClient::sendVerificationCode(const QString& toEmail, const QString& code) {
    QString subject = QString::fromUtf8("Код подтверждения TaMP");
    QString body    = QString::fromUtf8("Ваш код подтверждения: ") + code;
    return doSend(toEmail, subject, body);
}

bool SmtpClient::sendPasswordResetCode(const QString& toEmail,
                                        const QString& login,
                                        const QString& code) {
    QString subject = QString::fromUtf8("Восстановление пароля TaMP");
    QString body    = QString::fromUtf8("Здравствуйте, ") + login +
                      QString::fromUtf8(".\n\nВаш код для сброса пароля: ") + code +
                      QString::fromUtf8("\n\nЕсли вы не запрашивали сброс пароля - игнорируйте это письмо.");
    return doSend(toEmail, subject, body);
}