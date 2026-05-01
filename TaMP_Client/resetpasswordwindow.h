#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QStackedWidget>

/**
 * @file resetpasswordwindow.h
 * @brief Окно восстановления пароля.
 */

/**
 * @class ResetPasswordWindow
 * @brief Интерфейс для восстановления пароля в несколько шагов.
 */
class ResetPasswordWindow : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Конструктор окна восстановления пароля.
     * @param parent Родительский виджет.
     */
    explicit ResetPasswordWindow(QWidget* parent = nullptr);

signals:
    /**
     * @brief Переход к окну авторизации.
     */
    void goToLogin();

private slots:
    /**
     * @brief Обработчик отправки кода.
     */
    void onSendCodeClicked();

    /**
     * @brief Обработчик подтверждения кода.
     */
    void onConfirmClicked();

    /**
     * @brief Обработчик установки нового пароля.
     */
    void onSetPasswordClicked();

    /**
     * @brief Обработка ответа сервера.
     * @param message Сообщение.
     */
    void onMessageReceived(const QString& message);

private:
    QStackedWidget* stack; ///< Контейнер шагов

    /// Шаг 1: ввод логина
    QWidget*     step1;
    QLineEdit*   editLogin;
    QPushButton* btnSendCode;
    QLabel*      labelStatus1;
    QLabel*      labelMaskedEmail;

    QString      currentEmail; ///< Email пользователя (если используется)

    /// Шаг 2: ввод кода
    QWidget*     step2;
    QLineEdit*   editCode;
    QPushButton* btnConfirm;
    QLabel*      labelStatus2;

    /// Шаг 3: новый пароль
    QWidget*     step3;
    QLineEdit*   editNewPassword;
    QLineEdit*   editNewPassword2;
    QPushButton* btnSetPassword;
    QLabel*      labelStatus3;

    QPushButton* btnBack; ///< Кнопка возврата

    QString currentLogin; ///< Текущий логин пользователя

    /**
     * @brief Создает шаг 1.
     */
    QWidget* makeStep1();

    /**
     * @brief Создает шаг 2.
     */
    QWidget* makeStep2();

    /**
     * @brief Создает шаг 3.
     */
    QWidget* makeStep3();
};