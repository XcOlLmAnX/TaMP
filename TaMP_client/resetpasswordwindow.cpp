#include "resetpasswordwindow.h"
#include "tcpclient.h"
#include "md5_util.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>

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

QWidget* ResetPasswordWindow::makeStep1() {
    step1 = new QWidget();
    editEmail   = new QLineEdit();
    btnSendCode = new QPushButton("Отправить код");
    labelStatus1 = new QLabel("");
    labelStatus1->setAlignment(Qt::AlignCenter);

    editEmail->setPlaceholderText("Введите email");

    connect(btnSendCode, &QPushButton::clicked, this, &ResetPasswordWindow::onSendCodeClicked);

    QVBoxLayout* l = new QVBoxLayout(step1);
    l->addSpacing(10);
    l->addWidget(new QLabel("Введите email для восстановления:"));
    l->addWidget(editEmail);
    l->addWidget(labelStatus1);
    l->addWidget(btnSendCode);
    return step1;
}

QWidget* ResetPasswordWindow::makeStep2() {
    step2 = new QWidget();
    editCode   = new QLineEdit();
    btnConfirm = new QPushButton("Подтвердить");
    labelStatus2 = new QLabel("");
    labelStatus2->setAlignment(Qt::AlignCenter);

    editCode->setPlaceholderText("Введите 6-значный код");

    connect(btnConfirm, &QPushButton::clicked, this, &ResetPasswordWindow::onConfirmClicked);

    QVBoxLayout* l = new QVBoxLayout(step2);
    l->addSpacing(10);
    l->addWidget(new QLabel("Код отправлен на вашу почту:"));
    l->addWidget(editCode);
    l->addWidget(labelStatus2);
    l->addWidget(btnConfirm);
    return step2;
}

QWidget* ResetPasswordWindow::makeStep3() {
    step3 = new QWidget();
    editNewPassword  = new QLineEdit();
    editNewPassword2 = new QLineEdit();
    btnSetPassword   = new QPushButton("Сохранить пароль");
    labelStatus3     = new QLabel("");
    labelStatus3->setAlignment(Qt::AlignCenter);

    editNewPassword->setEchoMode(QLineEdit::Password);
    editNewPassword2->setEchoMode(QLineEdit::Password);
    editNewPassword->setPlaceholderText("Новый пароль");
    editNewPassword2->setPlaceholderText("Повторите пароль");

    connect(btnSetPassword, &QPushButton::clicked, this, &ResetPasswordWindow::onSetPasswordClicked);

    QVBoxLayout* l = new QVBoxLayout(step3);
    l->addSpacing(10);
    l->addWidget(editNewPassword);
    l->addWidget(editNewPassword2);
    l->addWidget(labelStatus3);
    l->addWidget(btnSetPassword);
    return step3;
}

void ResetPasswordWindow::onSendCodeClicked() {
    QString email = editEmail->text().trimmed();
    if (email.isEmpty()) {
        labelStatus1->setText("Введите email");
        return;
    }
    currentEmail = email;
    labelStatus1->setText("Отправка...");
    labelStatus1->setStyleSheet("color: gray;");
    TcpClient::getInstance()->sendMessage(QString("reset&%1\n").arg(email));
}

void ResetPasswordWindow::onConfirmClicked() {
    QString code = editCode->text().trimmed();
    if (code.isEmpty()) {
        labelStatus2->setText("Введите код");
        return;
    }
    TcpClient::getInstance()->sendMessage(QString("confirm&%1&%2\n").arg(currentEmail, code));
}

void ResetPasswordWindow::onSetPasswordClicked() {
    QString p1 = editNewPassword->text().trimmed();
    QString p2 = editNewPassword2->text().trimmed();

    if (p1.isEmpty() || p2.isEmpty()) {
        labelStatus3->setText("Заполните оба поля");
        return;
    }
    if (p1 != p2) {
        labelStatus3->setText("Пароли не совпадают");
        labelStatus3->setStyleSheet("color: red;");
        return;
    }

    QString hash = QString::fromStdString(md5(p1.toStdString()));
    TcpClient::getInstance()->sendMessage(
        QString("newpass&%1&%2\n").arg(currentEmail, hash));
}

void ResetPasswordWindow::onMessageReceived(const QString& message) {
    if (message.startsWith("reset+")) {
        labelStatus1->setText("Код отправлен!");
        labelStatus1->setStyleSheet("color: green;");
        stack->setCurrentIndex(1);
    } else if (message.startsWith("reset-")) {
        labelStatus1->setText("Email не найден");
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