#include "tcpclient.h"

using namespace std;

/**
 * @file tcpclient.cpp
 * @brief Реализация сетевого Singleton слоя клиента.
 */

// Инициализация статического экземпляра
TcpClient* TcpClient::instance = nullptr;

TcpClient* TcpClient::getInstance() {
    if (instance == nullptr) {
        instance = new TcpClient();
    }
    return instance;
}

TcpClient::TcpClient(QObject* parent) : QObject(parent) {
    socket = new QTcpSocket(this);

    connect(socket, &QTcpSocket::connected,
            this,   &TcpClient::connected);

    connect(socket, &QTcpSocket::disconnected,
            this,   &TcpClient::disconnected);

    connect(socket, &QTcpSocket::readyRead,
            this,   &TcpClient::onReadyRead);

    connect(socket, &QAbstractSocket::errorOccurred,
            this,   &TcpClient::onError);
}

void TcpClient::connectToServer(const QString& host, int port) {
    socket->connectToHost(host, port);
}

void TcpClient::sendMessage(const QString& message) {
    if (isConnected()) {
        socket->write(message.toUtf8());
    }
}

bool TcpClient::isConnected() const {
    return socket->state() == QAbstractSocket::ConnectedState;
}

void TcpClient::onReadyRead() {
    while (socket->canReadLine()) {
        QString line = QString::fromUtf8(socket->readLine()).trimmed();
        if (!line.isEmpty()) {
            emit messageReceived(line);
        }
    }
}

void TcpClient::onError(QAbstractSocket::SocketError error) {
    Q_UNUSED(error)
    emit errorOccurred(socket->errorString());
}
