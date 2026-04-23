#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QStackedWidget>

class ResetPasswordWindow : public QWidget {
    Q_OBJECT

public:
    explicit ResetPasswordWindow(QWidget* parent = nullptr);

    signals:
        void goToLogin();

private slots:
    void onSendCodeClicked();
    void onConfirmClicked();
    void onSetPasswordClicked();
    void onMessageReceived(const QString& message);

private:
    QStackedWidget* stack;

    QWidget*     step1;
    QLineEdit*   editEmail;
    QPushButton* btnSendCode;
    QLabel*      labelStatus1;

    QWidget*     step2;
    QLineEdit*   editCode;
    QPushButton* btnConfirm;
    QLabel*      labelStatus2;

    QWidget*     step3;
    QLineEdit*   editNewPassword;
    QLineEdit*   editNewPassword2;
    QPushButton* btnSetPassword;
    QLabel*      labelStatus3;

    QPushButton* btnBack;

    QString currentEmail;

    QWidget* makeStep1();
    QWidget* makeStep2();
    QWidget* makeStep3();
};