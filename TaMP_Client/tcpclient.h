#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QString>

/**
 * @file tcpclient.h
 * @brief Синглтон-обёртка над QTcpSocket для TCP-связи с сервером TaMP.
 */

/**
 * @class TcpClient
 * @brief Единственный объект TCP-соединения в клиентском приложении.
 *
 * @details Реализует паттерн Singleton: конструктор приватный, копирование и
 * присваивание запрещены, единственный экземпляр доступен через getInstance().
 * Все окна приложения разделяют один сокет и получают входящие данные через
 * сигнал messageReceived(). Данные читаются построчно — одна строка = один ответ сервера.
 *
 * @note Потокобезопасность не гарантируется; весь ввод-вывод происходит
 * в главном потоке Qt.
 *
 * @see LoginWindow, RegisterWindow, ResetPasswordWindow
 */
class TcpClient : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Возвращает единственный экземпляр TcpClient, создавая его при первом вызове.
     * @return Указатель на глобальный экземпляр TcpClient.
     */
    static TcpClient* getInstance();

    TcpClient(const TcpClient&)            = delete;
    TcpClient& operator=(const TcpClient&) = delete;

    /**
     * @brief Инициирует асинхронное TCP-подключение к серверу.
     *
     * @details Вызов неблокирующий. Результат подключения сообщается через
     * сигнал connected() при успехе или errorOccurred() при ошибке.
     *
     * @param host Имя хоста или IP-адрес сервера (например, "127.0.0.1").
     * @param port Порт сервера (1–65535).
     */
    void connectToServer(const QString& host, int port);

    /**
     * @brief Отправляет строку на сервер в кодировке UTF-8.
     *
     * @details Запись в сокет выполняется только если соединение активно
     * (isConnected() == true). Строка должна заканчиваться на '\\n' согласно
     * текстовому протоколу TaMP.
     *
     * @param message Сообщение в формате протокола TaMP.
     */
    void sendMessage(const QString& message);

    /**
     * @brief Проверяет активность TCP-соединения.
     * @return @c true, если сокет находится в состоянии QAbstractSocket::ConnectedState.
     */
    bool isConnected() const;

signals:
    /**
     * @brief Испускается при получении одной полной строки от сервера.
     *
     * @details Строка передаётся без завершающего символа '\\n'. Все окна
     * подключаются к этому сигналу и фильтруют сообщения по префиксу.
     *
     * @param message Одна строка ответа сервера.
     */
    void messageReceived(const QString& message);

    /**
     * @brief Испускается при успешном установлении TCP-соединения с сервером.
     */
    void connected();

    /**
     * @brief Испускается при разрыве TCP-соединения с сервером.
     */
    void disconnected();

    /**
     * @brief Испускается при возникновении ошибки сокета.
     * @param error Текст ошибки от QAbstractSocket::errorString().
     */
    void errorOccurred(const QString& error);

private:
    /**
     * @brief Приватный конструктор (паттерн Singleton).
     * @param parent Родительский QObject (обычно nullptr).
     */
    explicit TcpClient(QObject* parent = nullptr);

    static TcpClient* instance; ///< Единственный экземпляр синглтона.

    QTcpSocket* socket; ///< Сокет для обмена данными с сервером.

private slots:
    /**
     * @brief Читает из сокета все доступные строки и испускает messageReceived().
     *
     * @details Вызывается автоматически по сигналу QTcpSocket::readyRead.
     * Пустые строки игнорируются.
     */
    void onReadyRead();

    /**
     * @brief Транслирует код ошибки сокета в сигнал errorOccurred() с текстом.
     * @param error Код ошибки QAbstractSocket (не используется напрямую — текст берётся
     *              из socket->errorString()).
     */
    void onError(QAbstractSocket::SocketError error);
};
