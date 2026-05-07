#include "registerwindow.h"
#include "tcpclient.h"
#include "md5_util.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>

/**
 * @file registerwindow.cpp
 * @brief Реализация окна регистрации RegisterWindow.
 */

/**
 * @brief Создаёт виджеты, компонует layout и подключает сигналы.
 */
RegisterWindow::RegisterWindow(QWidget* parent) : QWidget(parent) {
    setWindowTitle("Регистрация");
    setFixedSize(350, 320);

    editLogin    = new QLineEdit();
    editName     = new QLineEdit();
    editPassword = new QLineEdit();
    editEmail    = new QLineEdit();
    editPassword->setEchoMode(QLineEdit::Password);

    editLogin->setPlaceholderText("Придумайте логин");
    editName->setPlaceholderText("Введите имя");
    editPassword->setPlaceholderText("Придумайте пароль");
    editEmail->setPlaceholderText("Ваш email");

    btnRegister = new QPushButton("Зарегистрироваться");
    btnBack     = new QPushButton("Назад");

    labelStatus = new QLabel("");
    labelStatus->setAlignment(Qt::AlignCenter);
    labelStatus->setStyleSheet("color: red;");

    QFormLayout* formLayout = new QFormLayout();
    formLayout->addRow("Логин:",  editLogin);
    formLayout->addRow("Имя:",    editName);
    formLayout->addRow("Пароль:", editPassword);
    formLayout->addRow("Email:",  editEmail);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addWidget(btnRegister);
    btnLayout->addWidget(btnBack);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addSpacing(20);
    mainLayout->addLayout(formLayout);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(labelStatus);
    mainLayout->addLayout(btnLayout);
    mainLayout->addSpacing(20);

    connect(btnRegister, &QPushButton::clicked, this, &RegisterWindow::onRegisterClicked);
    connect(btnBack,     &QPushButton::clicked, this, &RegisterWindow::goToLogin);
    connect(TcpClient::getInstance(), &TcpClient::messageReceived,
            this, &RegisterWindow::onMessageReceived);
}

/**
 * @brief Валидирует форму и отправляет запрос регистрации серверу.
 *
 * @details Проверяет: заполненность всех полей, корректность формата email
 * (наличие '@' и домена с точкой), длину пароля (≥ 6 символов) и наличие
 * хотя бы одной цифры в пароле. При прохождении всех проверок отправляет
 * серверу запрос: `reg&{login}&{name}&{md5hash}&{email}\n`.
 */
void RegisterWindow::onRegisterClicked() {
    QString login    = editLogin->text().trimmed();
    QString name     = editName->text().trimmed();
    QString password = editPassword->text().trimmed();
    QString email    = editEmail->text().trimmed();

    if (login.isEmpty()) {
        labelStatus->setText("Поле «Логин» не заполнено");
        labelStatus->setStyleSheet("color: red;");
        editLogin->setFocus();
        return;
    }
    if (name.isEmpty()) {
        labelStatus->setText("Поле «Имя» не заполнено");
        labelStatus->setStyleSheet("color: red;");
        editName->setFocus();
        return;
    }
    if (password.isEmpty()) {
        labelStatus->setText("Поле «Пароль» не заполнено");
        labelStatus->setStyleSheet("color: red;");
        editPassword->setFocus();
        return;
    }
    if (email.isEmpty()) {
        labelStatus->setText("Поле «Email» не заполнено");
        labelStatus->setStyleSheet("color: red;");
        editEmail->setFocus();
        return;
    }

    if (!email.contains('@') || email.section('@', 1).isEmpty() || !email.section('@', 1).contains('.')) {
        labelStatus->setText("Введите корректный email");
        labelStatus->setStyleSheet("color: red;");
        editEmail->setFocus();
        return;
    }

    if (password.length() < 6) {
        labelStatus->setText("Пароль должен быть не менее 6 символов");
        labelStatus->setStyleSheet("color: red;");
        editPassword->setFocus();
        return;
    }

    bool hasDigit = false;
    for (QChar c : password)
        if (c.isDigit()) { hasDigit = true; break; }

    if (!hasDigit) {
        labelStatus->setText("Пароль должен содержать хотя бы одну цифру");
        labelStatus->setStyleSheet("color: red;");
        editPassword->setFocus();
        return;
    }

    QString hash = QString::fromStdString(md5(password.toStdString()));

    labelStatus->setText("Отправка...");
    labelStatus->setStyleSheet("color: gray;");

    TcpClient::getInstance()->sendMessage(
        QString("reg&%1&%2&%3&%4\n").arg(login, name, hash, email));
}

/**
 * @brief Обрабатывает ответы сервера с префиксом «reg».
 *
 * @details Формат успешного ответа: `reg+&{login}`.
 * Формат ошибки (логин занят): `reg-`.
 *
 * @param message Строка ответа от сервера.
 */
void RegisterWindow::onMessageReceived(const QString& message) {
    if (!message.startsWith("reg")) return;

    if (message.startsWith("reg+")) {
        labelStatus->setText("Регистрация успешна! Войдите.");
        labelStatus->setStyleSheet("color: green;");
        emit registerSuccess();
    } else {
        labelStatus->setText("Логин уже занят");
        labelStatus->setStyleSheet("color: red;");
    }
}
