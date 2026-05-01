#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

/**
 * @file loginwindow.h
 * @brief Окно авторизации пользователя.
 */

/**
 * @class LoginWindow
 * @brief Интерфейс для входа пользователя в систему.
 */
class LoginWindow : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Конструктор окна авторизации.
     * @param parent Родительский виджет.
     */
    explicit LoginWindow(QWidget* parent = nullptr);

    signals:
        /**
         * @brief Сигнал успешного входа.
         * @param name Имя пользователя.
         */
        void loginSuccess(const QString& name);

    /**
     * @brief Переход к окну регистрации.
     */
    void goToRegister();

    /**
     * @brief Переход к окну восстановления пароля.
     */
    void goToResetPassword();

private slots:
    /**
     * @brief Обработчик нажатия кнопки входа.
     */
    void onLoginClicked();

    /**
     * @brief Обработка ответа от сервера.
     * @param message Сообщение.
     */
    void onMessageReceived(const QString& message);

private:
    QLineEdit*   editLogin;    ///< Поле ввода логина
    QLineEdit*   editPassword; ///< Поле ввода пароля
    QPushButton* btnLogin;     ///< Кнопка входа
    QPushButton* btnRegister;  ///< Кнопка перехода к регистрации
    QPushButton* btnForgot;    ///< Кнопка восстановления пароля
    QLabel*      labelStatus;  ///< Статус (ошибка/успех)
};