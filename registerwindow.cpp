#include "registerwindow.h"
#include "tcpclient.h"
#include "md5_util.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>

RegisterWindow::RegisterWindow(QWidget* parent) : QWidget(parent) {
    setWindowTitle("TaMP — Регистрация");
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
    formLayout->addRow("Имя:",    editName);      // новое поле
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

void RegisterWindow::onRegisterClicked() {
    QString login    = editLogin->text().trimmed();
    QString name     = editName->text().trimmed();
    QString password = editPassword->text().trimmed();
    QString email    = editEmail->text().trimmed();

    if (login.isEmpty() || name.isEmpty() || password.isEmpty() || email.isEmpty()) {
        labelStatus->setText("Заполните все поля");
        return;
    }

    // MD5 пароля перед отправкой
    QString hash = QString::fromStdString(md5(password.toStdString()));

    labelStatus->setText("Отправка...");
    labelStatus->setStyleSheet("color: gray;");

    // reg&login&name&hash&email
    QString message = QString("reg&%1&%2&%3&%4\n").arg(login, name, hash, email);
    TcpClient::getInstance()->sendMessage(message);
}

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