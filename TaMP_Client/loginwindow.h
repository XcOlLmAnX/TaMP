#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

/**
 * @file loginwindow.h
 * @brief Окно авторизации пользователя в приложении TaMP.
 */

/**
 * @class LoginWindow
 * @brief Форма входа в систему с полями логина и пароля.
 *
 * @details Отправляет серверу запрос вида <tt>auth&<login>&<md5(password)>\n</tt>
 * и обрабатывает ответы с префиксами <tt>auth+</tt> (успех) и <tt>auth-</tt> (ошибка).
 * При успешном входе испускает loginSuccess() с именем пользователя.
 *
 * @see TcpClient, RegisterWindow, ResetPasswordWindow
 */
class LoginWindow : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Создаёт и настраивает виджет окна авторизации.
     * @param parent Родительский виджет (обычно nullptr — окно верхнего уровня).
     */
    explicit LoginWindow(QWidget* parent = nullptr);

signals:
    /**
     * @brief Испускается после успешной авторизации на сервере.
     * @param name Отображаемое имя вошедшего пользователя.
     */
    void loginSuccess(const QString& name);

    /**
     * @brief Испускается при нажатии кнопки перехода к регистрации.
     */
    void goToRegister();

    /**
     * @brief Испускается при нажатии кнопки «Забыли пароль?».
     */
    void goToResetPassword();

private slots:
    /**
     * @brief Валидирует поля, хэширует пароль и отправляет запрос авторизации серверу.
     */
    void onLoginClicked();

    /**
     * @brief Обрабатывает ответ сервера; реагирует только на строки с префиксом «auth».
     * @param message Строка ответа от TcpClient::messageReceived().
     */
    void onMessageReceived(const QString& message);

private:
    QLineEdit*   editLogin;    ///< Поле ввода логина.
    QLineEdit*   editPassword; ///< Поле ввода пароля (скрытый ввод).
    QPushButton* btnLogin;     ///< Кнопка отправки запроса авторизации.
    QPushButton* btnRegister;  ///< Кнопка перехода к форме регистрации.
    QPushButton* btnForgot;    ///< Кнопка перехода к восстановлению пароля.
    QLabel*      labelStatus;  ///< Метка для отображения статуса операции или ошибки.
};
