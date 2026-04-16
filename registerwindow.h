#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

class RegisterWindow : public QWidget {
    Q_OBJECT

public:
    explicit RegisterWindow(QWidget* parent = nullptr);

    signals:
        void registerSuccess();
    void goToLogin();

private slots:
    void onRegisterClicked();
    void onMessageReceived(const QString& message);

private:
    QLineEdit*   editLogin;
    QLineEdit*   editName;      ///< Поле имени
    QLineEdit*   editPassword;
    QLineEdit*   editEmail;
    QPushButton* btnRegister;
    QPushButton* btnBack;
    QLabel*      labelStatus;
};