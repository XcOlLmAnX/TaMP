#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

/**
 * @file loginwindow.h
 * @brief Форма авторизации пользователя (View слой MVC).
 *
 * Отправляет запрос auth&login&password через TcpClient,
 * переключается на главный экран при успешном ответе.
 */
class LoginWindow : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Конструктор формы авторизации.
     * @param parent Родительский виджет.
     */
    explicit LoginWindow(QWidget* parent = nullptr);

signals:
    /// @brief Успешная авторизация — переключиться на TaskWindow.
    void loginSuccess(const QString& login);

    /// @brief Нажата кнопка "Регистрация" — переключиться на RegisterWindow.
    void goToRegister();

private slots:
    /// @brief Отправляет запрос авторизации на сервер.
    void onLoginClicked();

    /// @brief Обрабатывает ответ сервера.
    void onMessageReceived(const QString& message);

private:
    QLineEdit*   editLogin;     ///< Поле ввода логина
    QLineEdit*   editPassword;  ///< Поле ввода пароля
    QPushButton* btnLogin;      ///< Кнопка "Войти"
    QPushButton* btnRegister;   ///< Кнопка "Регистрация"
    QLabel*      labelStatus;   ///< Статус (ошибка/успех)
};
