#include "loginwindow.h"
#include "md5_util.h"
#include "tcpclient.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>

using namespace std;

LoginWindow::LoginWindow(QWidget* parent) : QWidget(parent) {
    setWindowTitle("Авторизация");
    setFixedSize(350, 250);

    editLogin    = new QLineEdit();
    editPassword = new QLineEdit();
    editPassword->setEchoMode(QLineEdit::Password);
    editLogin->setPlaceholderText("Введите логин");
    editPassword->setPlaceholderText("Введите пароль");

    btnLogin = new QPushButton("Войти");

    btnRegister = new QPushButton("Нет аккаунта? Зарегистрироваться");
    btnRegister->setFlat(true);
    btnRegister->setStyleSheet("color: gray; text-decoration: underline;");

    btnForgot = new QPushButton("Забыли пароль?");
    btnForgot->setFlat(true);
    btnForgot->setStyleSheet("color: gray; text-decoration: underline;");

    labelStatus = new QLabel("");
    labelStatus->setAlignment(Qt::AlignCenter);
    labelStatus->setStyleSheet("color: red;");

    QFormLayout* formLayout = new QFormLayout();
    formLayout->addRow("Логин:",  editLogin);
    formLayout->addRow("Пароль:", editPassword);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addSpacing(20);
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(btnRegister, 0, Qt::AlignLeft);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(labelStatus);
    mainLayout->addWidget(btnLogin);
    mainLayout->addSpacing(5);
    mainLayout->addWidget(btnForgot,   0, Qt::AlignCenter);

    connect(btnLogin,    &QPushButton::clicked, this, &LoginWindow::onLoginClicked);
    connect(btnRegister, &QPushButton::clicked, this, &LoginWindow::goToRegister);
    connect(btnForgot,   &QPushButton::clicked, this, &LoginWindow::goToResetPassword);
    connect(TcpClient::getInstance(), &TcpClient::messageReceived,
            this, &LoginWindow::onMessageReceived);
}

void LoginWindow::onLoginClicked() {
    QString login    = editLogin->text().trimmed();
    QString password = editPassword->text().trimmed();

    if (login.isEmpty()) {
        labelStatus->setText("Поле «Логин» не заполнено");
        labelStatus->setStyleSheet("color: red;");
        editLogin->setFocus();
        return;
    }
    if (password.isEmpty()) {
        labelStatus->setText("Поле «Пароль» не заполнено");
        labelStatus->setStyleSheet("color: red;");
        editPassword->setFocus();
        return;
    }

    QString hash = QString::fromStdString(md5(password.toStdString()));
    labelStatus->setText("Подключение...");
    labelStatus->setStyleSheet("color: gray;");
    TcpClient::getInstance()->sendMessage(QString("auth&%1&%2\n").arg(login, hash));
}

void LoginWindow::onMessageReceived(const QString& message) {
    if (!message.startsWith("auth")) return;

    if (message.startsWith("auth+")) {
        QString name = message.section('&', 1, 1).trimmed();
        labelStatus->setText("Успешно!");
        labelStatus->setStyleSheet("color: green;");
        emit loginSuccess(name);
    } else if (message.startsWith("auth-")) {
        labelStatus->setText("Неверный логин или пароль");
        labelStatus->setStyleSheet("color: red;");
        editPassword->clear();
    }
}