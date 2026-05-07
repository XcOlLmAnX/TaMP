/**
 * @file tcpclient.cpp
 * @brief Реализация синглтон-клиента TcpClient.
 */

#include "tcpclient.h"

using namespace std;

TcpClient* TcpClient::instance = nullptr; ///< Статический указатель на единственный экземпляр.

/**
 * @brief Возвращает единственный экземпляр TcpClient, создавая его при первом вызове.
 */
TcpClient* TcpClient::getInstance() {
    if (instance == nullptr) {
        instance = new TcpClient();
    }
    return instance;
}

/**
 * @brief Создаёт сокет и подключает его сигналы к слотам этого класса.
 * @param parent Родительский QObject.
 */
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

/**
 * @brief Инициирует асинхронное подключение к серверу.
 * @param host Адрес сервера.
 * @param port Порт сервера.
 */
void TcpClient::connectToServer(const QString& host, int port) {
    socket->connectToHost(host, port);
}

/**
 * @brief Отправляет UTF-8 строку на сервер, если соединение активно.
 * @param message Сообщение для отправки.
 */
void TcpClient::sendMessage(const QString& message) {
    if (isConnected()) {
        socket->write(message.toUtf8());
    }
}

/**
 * @brief Проверяет, находится ли сокет в состоянии ConnectedState.
 * @return @c true при активном соединении.
 */
bool TcpClient::isConnected() const {
    return socket->state() == QAbstractSocket::ConnectedState;
}

/**
 * @brief Читает все доступные строки из сокета и испускает messageReceived().
 */
void TcpClient::onReadyRead() {
    while (socket->canReadLine()) {
        QString line = QString::fromUtf8(socket->readLine()).trimmed();
        if (!line.isEmpty()) {
            emit messageReceived(line);
        }
    }
}

/**
 * @brief Обрабатывает ошибку сокета: испускает errorOccurred() с текстом ошибки.
 * @param error Код ошибки (не используется, текст берётся из socket->errorString()).
 */
void TcpClient::onError(QAbstractSocket::SocketError error) {
    Q_UNUSED(error)
    emit errorOccurred(socket->errorString());
}
