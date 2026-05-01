#pragma once

#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include "qcustomplot.h"
#include <QPixmap>

/**
 * @file taskwindow.h
 * @brief Окно с графиком и численной задачей.
 */

/**
 * @class TaskWindow
 * @brief Интерфейс для работы с кусочной функцией, графиком и таблицей значений.
 */
class TaskWindow : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Конструктор окна задачи.
     * @param parent Родительский виджет.
     */
    explicit TaskWindow(QWidget* parent = nullptr);

    /**
     * @brief Устанавливает имя пользователя в заголовке окна.
     * @param name Имя пользователя.
     */
    void setName(const QString& name);

    signals:
        /**
         * @brief Сигнал выхода из аккаунта.
         */
        void logout();

private slots:
    /**
     * @brief Обновляет график и таблицу.
     */
    void updatePlot();

private:
    QCustomPlot*  plot;        ///< График функции
    QSlider*      sliderA;     ///< Параметр a
    QSlider*      sliderB;     ///< Параметр b
    QSlider*      sliderC;     ///< Параметр c

    QLabel*       labelA;      ///< Отображение значения a
    QLabel*       labelB;      ///< Отображение значения b
    QLabel*       labelC;      ///< Отображение значения c

    QPushButton*  btnLogout;   ///< Кнопка выхода
    QTableWidget* table;       ///< Таблица значений функции

    /**
     * @brief Создает строку со слайдером.
     */
    QHBoxLayout* makeSliderRow(const QString& name,
                                QSlider*& slider,
                                QLabel*& valueLabel);

    /**
     * @brief Кусочная функция.
     */
    double f(double x, int a, int b, int c);

    /**
     * @brief Настройка графика.
     */
    void setupPlot();

    /**
     * @brief Обновление таблицы значений.
     */
    void updateTable(int a, int b, int c);
};