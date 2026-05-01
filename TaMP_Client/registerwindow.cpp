#include "registerwindow.h"
#include "tcpclient.h"
#include "md5_util.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>

/**
 * @file registerwindow.cpp
 * @brief Реализация окна регистрации пользователя.
 */

/**
 * @brief Конструктор окна регистрации.
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
 * @brief Обработчик нажатия кнопки регистрации.
 */
void RegisterWindow::onRegisterClicked() {
    QString login    = editLogin->text().trimmed();
    QString name     = editName->text().trimmed();
    QString password = editPassword->text().trimmed();
    QString email    = editEmail->text().trimmed();

    /// Проверка заполнения полей
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

    /// Проверка корректности email
    if (!email.contains('@') || email.section('@', 1).isEmpty() || !email.section('@', 1).contains('.')) {
        labelStatus->setText("Введите корректный email");
        labelStatus->setStyleSheet("color: red;");
        editEmail->setFocus();
        return;
    }

    /// Проверка сложности пароля
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

    /// Хеширование пароля
    QString hash = QString::fromStdString(md5(password.toStdString()));

    labelStatus->setText("Отправка...");
    labelStatus->setStyleSheet("color: gray;");

    /// Отправка данных на сервер
    TcpClient::getInstance()->sendMessage(
        QString("reg&%1&%2&%3&%4\n").arg(login, name, hash, email));
}

/**
 * @brief Обработка ответа сервера.
 * @param message Сообщение от сервера.
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