#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

class LoginWindow : public QWidget {
    Q_OBJECT

public:
    explicit LoginWindow(QWidget* parent = nullptr);

    signals:
        void loginSuccess(const QString& name);
        void goToRegister();
        void goToResetPassword();

private slots:
    void onLoginClicked();
    void onMessageReceived(const QString& message);

private:
    QLineEdit*   editLogin;
    QLineEdit*   editPassword;
    QPushButton* btnLogin;
    QPushButton* btnRegister;
    QPushButton* btnForgot;
    QLabel*      labelStatus;
};