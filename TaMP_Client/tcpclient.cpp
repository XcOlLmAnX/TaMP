/**
 * @file tcpclient.cpp
 * @brief Реализация TCP-клиента для подключения к серверу и обмена сообщениями.
 */

#include "tcpclient.h"

using namespace std;

/**
 * @brief Указатель на единственный экземпляр TcpClient (Singleton).
 */
TcpClient* TcpClient::instance = nullptr;

/**
 * @brief Получить единственный экземпляр TcpClient.
 *
 * @return Указатель на TcpClient (Singleton).
 */
TcpClient* TcpClient::getInstance() {
    if (instance == nullptr) {
        instance = new TcpClient();
    }
    return instance;
}

/**
 * @brief Конструктор TcpClient.
 *
 * Инициализирует сокет и подключает сигналы Qt к слотам класса.
 *
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
 * @brief Установить соединение с сервером.
 *
 * @param host Адрес сервера.
 * @param port Порт сервера.
 */
void TcpClient::connectToServer(const QString& host, int port) {
    socket->connectToHost(host, port);
}

/**
 * @brief Отправить сообщение на сервер.
 *
 * Сообщение отправляется только если соединение активно.
 *
 * @param message Текст сообщения.
 */
void TcpClient::sendMessage(const QString& message) {
    if (isConnected()) {
        socket->write(message.toUtf8());
    }
}

/**
 * @brief Проверить, подключён ли клиент к серверу.
 *
 * @return true, если соединение установлено, иначе false.
 */
bool TcpClient::isConnected() const {
    return socket->state() == QAbstractSocket::ConnectedState;
}

/**
 * @brief Обработчик входящих данных от сервера.
 *
 * Считывает строки из сокета и генерирует сигнал messageReceived.
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
 * @brief Обработчик ошибок сокета.
 *
 * @param error Код ошибки (не используется напрямую).
 */
void TcpClient::onError(QAbstractSocket::SocketError error) {
    Q_UNUSED(error)
    emit errorOccurred(socket->errorString());
}