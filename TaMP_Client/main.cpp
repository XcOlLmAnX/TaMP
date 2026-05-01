#include <QApplication>
#include "tcpclient.h"
#include "loginwindow.h"
#include "registerwindow.h"
#include "taskwindow.h"
#include "resetpasswordwindow.h"

/**
 * @file main.cpp
 * @brief Точка входа в клиентское приложение.
 */

/**
 * @brief Главная функция клиента.
 * Инициализирует интерфейс и настраивает переходы между окнами.
 * @param argc Количество аргументов.
 * @param argv Аргументы командной строки.
 * @return Код завершения.
 */
int main(int argc, char* argv[]) {
    QApplication app(argc, argv);


    app.setStyleSheet(R"(
    QWidget {
        background-color: #1e1e2e;
        color: #cdd6f4;
        font-family: 'Segoe UI';
        font-size: 13px;
    }
    QLineEdit {
        background-color: #313244;
        border: 1px solid #45475a;
        border-radius: 6px;
        padding: 6px 10px;
        color: #cdd6f4;
    }
    QLineEdit:focus {
        border: 1px solid #89b4fa;
    }
    QLineEdit::placeholder {
        color: #6c7086;
    }
    QPushButton {
        background-color: #89b4fa;
        color: #1e1e2e;
        border: none;
        border-radius: 6px;
        padding: 7px 16px;
        font-weight: bold;
    }
    QPushButton:hover {
        background-color: #b4befe;
    }
    QPushButton:pressed {
        background-color: #74c7ec;
    }
    QPushButton:flat {
        background-color: transparent;
        color: #6c7086;
        font-weight: normal;
        text-decoration: underline;
        border: none;
        padding: 2px;
    }
    QPushButton:flat:hover {
        color: #89b4fa;
    }
    QLabel {
        background-color: transparent;
    }
    QFormLayout QLabel {
        color: #a6adc8;
    }
    QTableWidget {
        background-color: #181825;
        border: 1px solid #45475a;
        border-radius: 6px;
        gridline-color: #313244;
    }
    QTableWidget::item {
        padding: 4px;
    }
    QHeaderView::section {
        background-color: #313244;
        color: #89b4fa;
        border: none;
        padding: 5px;
        font-weight: bold;
    }
    QSlider::groove:horizontal {
        height: 4px;
        background: #45475a;
        border-radius: 2px;
    }
    QSlider::handle:horizontal {
        background: #89b4fa;
        width: 14px;
        height: 14px;
        margin: -5px 0;
        border-radius: 7px;
    }
    QSlider::sub-page:horizontal {
        background: #89b4fa;
        border-radius: 2px;
    }
    QScrollBar:vertical {
        background: #1e1e2e;
        width: 8px;
    }
    QScrollBar::handle:vertical {
        background: #45475a;
        border-radius: 4px;
    }
)");

    /// Подключение к серверу
    TcpClient::getInstance()->connectToServer("127.0.0.1", 12345);

    /// Создание окон
    LoginWindow*         loginWin    = new LoginWindow();
    RegisterWindow*      registerWin = new RegisterWindow();
    TaskWindow*          taskWin     = new TaskWindow();
    ResetPasswordWindow* resetWin    = new ResetPasswordWindow();

    /// Переход: логин → регистрация
    QObject::connect(loginWin, &LoginWindow::goToRegister, [=]() {
        loginWin->hide();
        registerWin->show();
    });

    /// Переход: регистрация → логин
    QObject::connect(registerWin, &RegisterWindow::goToLogin, [=]() {
        registerWin->hide();
        loginWin->show();
    });

    /// После успешной регистрации → логин
    QObject::connect(registerWin, &RegisterWindow::registerSuccess, [=]() {
        registerWin->hide();
        loginWin->show();
    });

    /// Переход: логин → восстановление пароля
    QObject::connect(loginWin, &LoginWindow::goToResetPassword, [=]() {
        loginWin->hide();
        resetWin->show();
    });

    /// Переход: восстановление → логин
    QObject::connect(resetWin, &ResetPasswordWindow::goToLogin, [=]() {
        resetWin->hide();
        loginWin->show();
    });

    /// Успешный вход → окно задач
    QObject::connect(loginWin, &LoginWindow::loginSuccess, [=](const QString& name) {
        taskWin->setName(name);
        loginWin->hide();
        taskWin->show();
    });

    /// Выход → обратно на логин
    QObject::connect(taskWin, &TaskWindow::logout, [=]() {
        QApplication::quit();
    });

    loginWin->show();
    return app.exec();
}