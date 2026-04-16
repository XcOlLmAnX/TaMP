#include "loginwindow.h"
#include "md5_util.h"
#include "tcpclient.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>

using namespace std;

/**
 * @file loginwindow.cpp
 * @brief Реализация формы авторизации.
 */

LoginWindow::LoginWindow(QWidget* parent) : QWidget(parent) {
    setWindowTitle("TaMP — Авторизация");
    setFixedSize(350, 250);

    // ── Поля ввода ────────────────────────────────────────────────────────────
    editLogin    = new QLineEdit();
    editPassword = new QLineEdit();
    editPassword->setEchoMode(QLineEdit::Password);
    editLogin->setPlaceholderText("Введите логин");
    editPassword->setPlaceholderText("Введите пароль");

    // ── Кнопки ───────────────────────────────────────────────────────────────
    btnLogin    = new QPushButton("Войти");
    btnRegister = new QPushButton("Регистрация");

    // ── Статус ───────────────────────────────────────────────────────────────
    labelStatus = new QLabel("");
    labelStatus->setAlignment(Qt::AlignCenter);
    labelStatus->setStyleSheet("color: red;");

    // ── Компоновка ────────────────────────────────────────────────────────────
    QFormLayout* formLayout = new QFormLayout();
    formLayout->addRow("Логин:",   editLogin);
    formLayout->addRow("Пароль:",  editPassword);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addWidget(btnLogin);
    btnLayout->addWidget(btnRegister);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addSpacing(20);
    mainLayout->addLayout(formLayout);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(labelStatus);
    mainLayout->addLayout(btnLayout);
    mainLayout->addSpacing(20);

    // ── Сигналы ───────────────────────────────────────────────────────────────
    connect(btnLogin,    &QPushButton::clicked,
            this,        &LoginWindow::onLoginClicked);

    connect(btnRegister, &QPushButton::clicked,
            this,        &LoginWindow::goToRegister);

    connect(TcpClient::getInstance(), &TcpClient::messageReceived,
            this,                     &LoginWindow::onMessageReceived);
}

void LoginWindow::onLoginClicked() {
    QString login    = editLogin->text().trimmed();
    QString password = editPassword->text().trimmed();

    if (login.isEmpty() || password.isEmpty()) {
        labelStatus->setText("Заполните все поля");
        return;
    }

    QString hash = QString::fromStdString(md5(password.toStdString()));

    labelStatus->setText("Подключение...");
    labelStatus->setStyleSheet("color: gray;");

    QString message = QString("auth&%1&%2\n").arg(login, hash);
    TcpClient::getInstance()->sendMessage(message);
}

void LoginWindow::onMessageReceived(const QString& message) {
    // Обрабатываем только ответы на auth
    if (!message.startsWith("auth")) return;

    if (message.startsWith("auth+")) {
        // auth+&login — извлекаем логин
        QString login = message.section('&', 1, 1);
        labelStatus->setText("Успешно!");
        labelStatus->setStyleSheet("color: green;");
        emit loginSuccess(login);
    } else if (message.startsWith("auth-")) {
        labelStatus->setText("Неверный логин или пароль");
        labelStatus->setStyleSheet("color: red;");
        editPassword->clear();
    }
}
