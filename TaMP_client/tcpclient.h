#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QString>

/**
 * @file tcpclient.h
 * @brief Сетевой слой клиента — Singleton обёртка над QTcpSocket.
 *
 * Реализует улучшенный Singleton: приватный конструктор,
 * удалённые копирование и присваивание, статический метод getInstance().
 */
class TcpClient : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Возвращает единственный экземпляр TcpClient.
     * @return Указатель на экземпляр.
     */
    static TcpClient* getInstance();

    // Запрещаем копирование и присваивание
    TcpClient(const TcpClient&)            = delete;
    TcpClient& operator=(const TcpClient&) = delete;

    /**
     * @brief Подключается к серверу.
     * @param host Адрес сервера.
     * @param port Порт сервера.
     */
    void connectToServer(const QString& host, int port);

    /**
     * @brief Отправляет строку на сервер.
     * @param message Сообщение по протоколу.
     */
    void sendMessage(const QString& message);

    /**
     * @brief Возвращает true если сокет подключён.
     */
    bool isConnected() const;

signals:
    /**
     * @brief Сигнал — получено сообщение от сервера.
     * @param message Строка ответа.
     */
    void messageReceived(const QString& message);

    /**
     * @brief Сигнал — соединение установлено.
     */
    void connected();

    /**
     * @brief Сигнал — соединение разорвано.
     */
    void disconnected();

    /**
     * @brief Сигнал — ошибка сокета.
     * @param error Текст ошибки.
     */
    void errorOccurred(const QString& error);

private:
    /// @brief Приватный конструктор (Singleton).
    explicit TcpClient(QObject* parent = nullptr);

    static TcpClient* instance;  ///< Единственный экземпляр

    QTcpSocket* socket;  ///< Сокет для связи с сервером

private slots:
    /// @brief Слот — читает данные из сокета.
    void onReadyRead();

    /// @brief Слот — обрабатывает ошибку сокета.
    void onError(QAbstractSocket::SocketError error);
};
