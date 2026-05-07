#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QStackedWidget>

/**
 * @file resetpasswordwindow.h
 * @brief Окно восстановления пароля через трёхшаговый мастер.
 */

/**
 * @class ResetPasswordWindow
 * @brief Мастер восстановления пароля из трёх шагов: ввод логина → код → новый пароль.
 *
 * @details Шаги переключаются через QStackedWidget:
 * - Шаг 1: пользователь вводит логин, сервер отправляет 6-значный код на email.
 * - Шаг 2: пользователь вводит полученный код подтверждения.
 * - Шаг 3: пользователь задаёт и подтверждает новый пароль.
 *
 * Все сетевые запросы идут через TcpClient. Ответы фильтруются по префиксам
 * `reset`, `confirm`, `newpass`.
 *
 * @see TcpClient, LoginWindow
 */
class ResetPasswordWindow : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Создаёт стек из трёх шагов и компонует основной layout.
     * @param parent Родительский виджет (обычно nullptr).
     */
    explicit ResetPasswordWindow(QWidget* parent = nullptr);

signals:
    /**
     * @brief Испускается при нажатии «Назад к входу» или после успешной смены пароля.
     */
    void goToLogin();

private slots:
    /**
     * @brief Отправляет логин серверу для запроса кода восстановления.
     *
     * @details Формат запроса: `reset&{login}\n`.
     * При пустом поле показывает ошибку, не отправляя запрос.
     */
    void onSendCodeClicked();

    /**
     * @brief Отправляет введённый код подтверждения серверу.
     *
     * @details Формат запроса: `confirm&{login}&{code}\n`.
     * Логин сохранён в currentLogin с первого шага.
     */
    void onConfirmClicked();

    /**
     * @brief Проверяет совпадение паролей, их сложность и отправляет MD5-хэш серверу.
     *
     * @details Формат запроса: `newpass&{login}&{md5hash}\n`.
     * Требования к паролю: ≥ 6 символов, хотя бы одна цифра.
     */
    void onSetPasswordClicked();

    /**
     * @brief Обрабатывает ответы сервера и управляет переключением шагов.
     *
     * @details Обрабатываемые префиксы:
     * - `reset+` — переход на шаг 2, отображение замаскированного email;
     * - `reset-` — ошибка: логин не найден;
     * - `confirm+` — переход на шаг 3;
     * - `confirm-` — ошибка: неверный код;
     * - `newpass+` — успех, сигнал goToLogin();
     * - `newpass-` — ошибка при сохранении пароля.
     *
     * @param message Строка ответа от TcpClient::messageReceived().
     */
    void onMessageReceived(const QString& message);

private:
    QStackedWidget* stack; ///< Контейнер, переключающий между шагами мастера.

    // --- Шаг 1: ввод логина ---
    QWidget*     step1;          ///< Виджет первого шага.
    QLineEdit*   editLogin;      ///< Поле ввода логина для восстановления.
    QPushButton* btnSendCode;    ///< Кнопка отправки запроса кода на email.
    QLabel*      labelStatus1;   ///< Метка статуса на первом шаге.
    QLabel*      labelMaskedEmail; ///< Отображение замаскированного email (шаг 2).

    // --- Шаг 2: ввод кода ---
    QWidget*     step2;          ///< Виджет второго шага.
    QLineEdit*   editCode;       ///< Поле ввода 6-значного кода подтверждения.
    QPushButton* btnConfirm;     ///< Кнопка подтверждения кода.
    QLabel*      labelStatus2;   ///< Метка статуса на втором шаге.

    // --- Шаг 3: новый пароль ---
    QWidget*     step3;           ///< Виджет третьего шага.
    QLineEdit*   editNewPassword;  ///< Поле ввода нового пароля.
    QLineEdit*   editNewPassword2; ///< Поле подтверждения нового пароля.
    QPushButton* btnSetPassword;   ///< Кнопка сохранения нового пароля.
    QLabel*      labelStatus3;     ///< Метка статуса на третьем шаге.

    QPushButton* btnBack;        ///< Кнопка возврата к форме авторизации.

    QString currentLogin; ///< Логин пользователя, сохранённый после первого шага.

    /**
     * @brief Создаёт виджет первого шага: ввод логина и запрос кода.
     * @return Готовый виджет шага 1.
     */
    QWidget* makeStep1();

    /**
     * @brief Создаёт виджет второго шага: ввод кода подтверждения.
     * @return Готовый виджет шага 2.
     */
    QWidget* makeStep2();

    /**
     * @brief Создаёт виджет третьего шага: ввод и подтверждение нового пароля.
     * @return Готовый виджет шага 3.
     */
    QWidget* makeStep3();
};
