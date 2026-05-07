#include "resetpasswordwindow.h"
#include "tcpclient.h"
#include "md5_util.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

/**
 * @file resetpasswordwindow.cpp
 * @brief Реализация мастера восстановления пароля ResetPasswordWindow.
 */

/**
 * @brief Создаёт стек шагов, кнопку возврата и подключает TcpClient.
 */
ResetPasswordWindow::ResetPasswordWindow(QWidget* parent) : QWidget(parent) {
    setWindowTitle("Восстановление пароля");
    setFixedSize(350, 220);

    stack = new QStackedWidget(this);
    stack->addWidget(makeStep1());
    stack->addWidget(makeStep2());
    stack->addWidget(makeStep3());

    btnBack = new QPushButton("Назад к входу");
    connect(btnBack, &QPushButton::clicked, this, &ResetPasswordWindow::goToLogin);

    QVBoxLayout* main = new QVBoxLayout(this);
    main->addWidget(stack);
    main->addWidget(btnBack);

    connect(TcpClient::getInstance(), &TcpClient::messageReceived,
            this, &ResetPasswordWindow::onMessageReceived);
}

/**
 * @brief Создаёт виджет шага 1: поле логина и кнопка «Отправить код».
 * @return Виджет шага 1.
 */
QWidget* ResetPasswordWindow::makeStep1() {
    step1 = new QWidget();
    editLogin    = new QLineEdit();
    btnSendCode  = new QPushButton("Отправить код");
    labelStatus1 = new QLabel("");
    labelStatus1->setAlignment(Qt::AlignCenter);

    editLogin->setPlaceholderText("Введите логин");

    connect(btnSendCode, &QPushButton::clicked,
            this, &ResetPasswordWindow::onSendCodeClicked);

    QVBoxLayout* l = new QVBoxLayout(step1);
    l->addSpacing(10);
    l->addWidget(new QLabel("Введите логин для восстановления:"));
    l->addWidget(editLogin);
    l->addWidget(labelStatus1);
    l->addWidget(btnSendCode);
    return step1;
}

/**
 * @brief Создаёт виджет шага 2: поле кода и кнопка «Подтвердить».
 * @return Виджет шага 2.
 */
QWidget* ResetPasswordWindow::makeStep2() {
    step2        = new QWidget();
    editCode     = new QLineEdit();
    btnConfirm   = new QPushButton("Подтвердить");
    labelStatus2 = new QLabel("");
    labelMaskedEmail = new QLabel("");
    labelStatus2->setAlignment(Qt::AlignCenter);
    labelMaskedEmail->setAlignment(Qt::AlignCenter);
    labelMaskedEmail->setStyleSheet("color: gray;");

    editCode->setPlaceholderText("Введите 6-значный код");

    connect(btnConfirm, &QPushButton::clicked,
            this, &ResetPasswordWindow::onConfirmClicked);

    QVBoxLayout* l = new QVBoxLayout(step2);
    l->addSpacing(10);
    l->addWidget(labelMaskedEmail);
    l->addWidget(editCode);
    l->addWidget(labelStatus2);
    l->addWidget(btnConfirm);
    return step2;
}

/**
 * @brief Создаёт виджет шага 3: два поля нового пароля и кнопка «Сохранить».
 * @return Виджет шага 3.
 */
QWidget* ResetPasswordWindow::makeStep3() {
    step3            = new QWidget();
    editNewPassword  = new QLineEdit();
    editNewPassword2 = new QLineEdit();
    btnSetPassword   = new QPushButton("Сохранить пароль");
    labelStatus3     = new QLabel("");
    labelStatus3->setAlignment(Qt::AlignCenter);

    editNewPassword->setEchoMode(QLineEdit::Password);
    editNewPassword2->setEchoMode(QLineEdit::Password);
    editNewPassword->setPlaceholderText("Новый пароль");
    editNewPassword2->setPlaceholderText("Повторите пароль");

    connect(btnSetPassword, &QPushButton::clicked,
            this, &ResetPasswordWindow::onSetPasswordClicked);

    QVBoxLayout* l = new QVBoxLayout(step3);
    l->addSpacing(10);
    l->addWidget(editNewPassword);
    l->addWidget(editNewPassword2);
    l->addWidget(labelStatus3);
    l->addWidget(btnSetPassword);
    return step3;
}

/**
 * @brief Проверяет поле логина и отправляет запрос <tt>reset&<login>\n</tt>.
 */
void ResetPasswordWindow::onSendCodeClicked() {
    QString login = editLogin->text().trimmed();
    if (login.isEmpty()) {
        labelStatus1->setText("Введите логин");
        labelStatus1->setStyleSheet("color: red;");
        return;
    }
    labelStatus1->setText("Отправка...");
    labelStatus1->setStyleSheet("color: gray;");
    TcpClient::getInstance()->sendMessage(QString("reset&%1\n").arg(login));
}

/**
 * @brief Проверяет поле кода и отправляет запрос <tt>confirm&<login>&<code>\n</tt>.
 */
void ResetPasswordWindow::onConfirmClicked() {
    QString code = editCode->text().trimmed();
    if (code.isEmpty()) {
        labelStatus2->setText("Введите код");
        labelStatus2->setStyleSheet("color: red;");
        return;
    }

    TcpClient::getInstance()->sendMessage(
        QString("confirm&%1&%2\n").arg(currentLogin, code));
}

/**
 * @brief Валидирует совпадение и сложность паролей, затем отправляет <tt>newpass&<login>&<md5>\n</tt>.
 */
void ResetPasswordWindow::onSetPasswordClicked() {
    QString p1 = editNewPassword->text().trimmed();
    QString p2 = editNewPassword2->text().trimmed();

    if (p1.isEmpty() || p2.isEmpty()) {
        labelStatus3->setText("Заполните оба поля");
        labelStatus3->setStyleSheet("color: red;");
        return;
    }
    if (p1 != p2) {
        labelStatus3->setText("Пароли не совпадают");
        labelStatus3->setStyleSheet("color: red;");
        return;
    }
    if (p1.length() < 6) {
        labelStatus3->setText("Пароль должен быть не менее 6 символов");
        labelStatus3->setStyleSheet("color: red;");
        return;
    }

    bool hasDigit = false;
    for (QChar c : p1)
        if (c.isDigit()) { hasDigit = true; break; }

    if (!hasDigit) {
        labelStatus3->setText("Пароль должен содержать хотя бы одну цифру");
        labelStatus3->setStyleSheet("color: red;");
        return;
    }

    QString hash = QString::fromStdString(md5(p1.toStdString()));
    TcpClient::getInstance()->sendMessage(
        QString("newpass&%1&%2\n").arg(currentLogin, hash));
}

/**
 * @brief Маршрутизирует ответы сервера по префиксам и обновляет UI или переключает шаг.
 * @param message Строка ответа от TcpClient::messageReceived().
 */
void ResetPasswordWindow::onMessageReceived(const QString& message) {
    if (message.startsWith("reset+")) {
        QString masked = message.section('&', 1, 1).trimmed();
        currentLogin = editLogin->text().trimmed();
        labelMaskedEmail->setText(
            QString("Код отправлен на почту: %1").arg(masked));
        labelStatus1->setText("");
        stack->setCurrentIndex(1);
    } else if (message.startsWith("reset-")) {
        labelStatus1->setText("Логин не найден");
        labelStatus1->setStyleSheet("color: red;");
    } else if (message.startsWith("confirm+")) {
        labelStatus2->setText("Код верный!");
        labelStatus2->setStyleSheet("color: green;");
        stack->setCurrentIndex(2);
    } else if (message.startsWith("confirm-")) {
        labelStatus2->setText("Неверный код");
        labelStatus2->setStyleSheet("color: red;");
    } else if (message.startsWith("newpass+")) {
        labelStatus3->setText("Пароль изменён!");
        labelStatus3->setStyleSheet("color: green;");
        emit goToLogin();
    } else if (message.startsWith("newpass-")) {
        labelStatus3->setText("Ошибка, попробуйте снова");
        labelStatus3->setStyleSheet("color: red;");
    }
}
