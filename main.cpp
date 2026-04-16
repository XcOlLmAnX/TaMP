#include <QApplication>
#include "tcpclient.h"
#include "loginwindow.h"
#include "registerwindow.h"
#include "taskwindow.h"

/**
 * @file main.cpp
 * @brief Точка входа клиентского приложения TaMP.
 */

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    TcpClient::getInstance()->connectToServer("127.0.0.1", 12345);

    LoginWindow*    loginWin    = new LoginWindow();
    RegisterWindow* registerWin = new RegisterWindow();
    TaskWindow*     taskWin     = new TaskWindow();

    // Login <-> Register
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

    // Login -> Task
    QObject::connect(loginWin, &LoginWindow::loginSuccess, [=](const QString& login) {
        taskWin->setName(login);
        loginWin->hide();
        taskWin->show();
    });

    // Task -> Login (выход)
    QObject::connect(taskWin, &TaskWindow::logout, [=]() {
        taskWin->hide();
        loginWin->show();
    });

    loginWin->show();
    return app.exec();
}