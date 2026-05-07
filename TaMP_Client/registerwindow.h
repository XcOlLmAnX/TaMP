#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

/**
 * @file registerwindow.h
 * @brief Окно регистрации нового пользователя в приложении TaMP.
 */

/**
 * @class RegisterWindow
 * @brief Форма создания нового аккаунта (логин, имя, пароль, email).
 *
 * @details Перед отправкой выполняет клиентскую валидацию:
 * проверяет заполненность полей, корректность формата email и сложность пароля
 * (минимум 6 символов, хотя бы одна цифра). Пароль передаётся серверу в виде
 * MD5-хэша. Формат запроса: `reg&{login}&{name}&{md5hash}&{email}\n`.
 *
 * @see TcpClient, LoginWindow
 */
class RegisterWindow : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Создаёт и компонует виджеты окна регистрации.
     * @param parent Родительский виджет (обычно nullptr).
     */
    explicit RegisterWindow(QWidget* parent = nullptr);

signals:
    /**
     * @brief Испускается после подтверждения сервером успешной регистрации.
     *
     * @details После этого сигнала приложение переводит пользователя на форму входа.
     */
    void registerSuccess();

    /**
     * @brief Испускается при нажатии кнопки «Назад» (возврат к LoginWindow).
     */
    void goToLogin();

private slots:
    /**
     * @brief Валидирует данные формы и отправляет запрос регистрации серверу.
     */
    void onRegisterClicked();

    /**
     * @brief Обрабатывает ответ сервера; реагирует только на строки с префиксом «reg».
     * @param message Строка ответа от TcpClient::messageReceived().
     */
    void onMessageReceived(const QString& message);

private:
    QLineEdit*   editLogin;    ///< Поле ввода желаемого логина.
    QLineEdit*   editName;     ///< Поле ввода отображаемого имени пользователя.
    QLineEdit*   editPassword; ///< Поле ввода пароля (скрытый ввод).
    QLineEdit*   editEmail;    ///< Поле ввода адреса электронной почты.

    QPushButton* btnRegister;  ///< Кнопка отправки формы регистрации.
    QPushButton* btnBack;      ///< Кнопка возврата к форме авторизации.

    QLabel*      labelStatus;  ///< Метка для отображения результата или ошибки валидации.
};
