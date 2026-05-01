#ifndef TAMP_TASKWINDOW_H
#define TAMP_TASKWINDOW_H

#pragma once

#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include "qcustomplot.h"

/**
 * @file taskwindow.h
 * @brief Окно задания с визуализацией функции и управлением параметрами.
 */

/**
 * @class TaskWindow
 * @brief Окно с графиком кусочной функции и слайдерами параметров.
 */
class TaskWindow : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Конструктор окна задания.
     * @param parent Родительский виджет.
     */
    explicit TaskWindow(QWidget* parent = nullptr);

    signals:
        /**
         * @brief Сигнал отправки ответа.
         * @param taskNumber Номер задания.
         * @param variant Вариант.
         * @param answer Ответ пользователя.
         */
        void answerSubmitted(int taskNumber, int variant, const QString& answer);

private slots:
    /**
     * @brief Обновление графика при изменении параметров.
     */
    void updatePlot();

private:
    QCustomPlot* plot; ///< График функции

    QSlider* sliderA; ///< Параметр a
    QSlider* sliderB; ///< Параметр b
    QSlider* sliderC; ///< Параметр c

    QLabel* labelA; ///< Отображение a
    QLabel* labelB; ///< Отображение b
    QLabel* labelC; ///< Отображение c

    /**
     * @brief Создает строку управления параметром.
     */
    QHBoxLayout* makeSliderRow(const QString& name,
                               const QString& formula,
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
};

#endif // TAMP_TASKWINDOW_H