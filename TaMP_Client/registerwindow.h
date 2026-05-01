#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

/**
 * @file registerwindow.h
 * @brief Окно регистрации пользователя.
 */

/**
 * @class RegisterWindow
 * @brief Интерфейс для создания нового аккаунта.
 */
class RegisterWindow : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Конструктор окна регистрации.
     * @param parent Родительский виджет.
     */
    explicit RegisterWindow(QWidget* parent = nullptr);

    signals:
        /**
         * @brief Сигнал успешной регистрации.
         */
        void registerSuccess();

    /**
     * @brief Переход к окну авторизации.
     */
    void goToLogin();

private slots:
    /**
     * @brief Обработчик нажатия кнопки регистрации.
     */
    void onRegisterClicked();

    /**
     * @brief Обработка ответа сервера.
     * @param message Сообщение.
     */
    void onMessageReceived(const QString& message);

private:
    QLineEdit*   editLogin;    ///< Поле логина
    QLineEdit*   editName;     ///< Поле имени
    QLineEdit*   editPassword; ///< Поле пароля
    QLineEdit*   editEmail;    ///< Поле email

    QPushButton* btnRegister;  ///< Кнопка регистрации
    QPushButton* btnBack;      ///< Кнопка возврата

    QLabel*      labelStatus;  ///< Статус (ошибка/успех)
};