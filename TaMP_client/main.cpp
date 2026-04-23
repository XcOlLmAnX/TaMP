#include <QApplication>
#include "tcpclient.h"
#include "loginwindow.h"
#include "registerwindow.h"
#include "taskwindow.h"
#include "resetpasswordwindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    TcpClient::getInstance()->connectToServer("127.0.0.1", 12345);

    LoginWindow*         loginWin    = new LoginWindow();
    RegisterWindow*      registerWin = new RegisterWindow();
    TaskWindow*          taskWin     = new TaskWindow();
    ResetPasswordWindow* resetWin    = new ResetPasswordWindow();

    QObject::connect(loginWin, &LoginWindow::goToRegister, [=]() {
        loginWin->hide();
        registerWin->show();
    });

    QObject::connect(registerWin, &RegisterWindow::goToLogin, [=]() {
        registerWin->hide();
        loginWin->show();
    });

    QObject::connect(registerWin, &RegisterWindow::registerSuccess, [=]() {
        registerWin->hide();
        loginWin->show();
    });

    QObject::connect(loginWin, &LoginWindow::goToResetPassword, [=]() {
        loginWin->hide();
        resetWin->show();
    });

    QObject::connect(resetWin, &ResetPasswordWindow::goToLogin, [=]() {
        resetWin->hide();
        loginWin->show();
    });

    QObject::connect(loginWin, &LoginWindow::loginSuccess, [=](const QString& name) {
        taskWin->setName(name);
        loginWin->hide();
        taskWin->show();
    });

    QObject::connect(taskWin, &TaskWindow::logout, [=]() {
        taskWin->hide();
        loginWin->show();
    });

    loginWin->show();
    return app.exec();
}