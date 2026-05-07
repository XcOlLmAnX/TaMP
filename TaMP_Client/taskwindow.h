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
 * @brief Главное окно приложения: интерактивный график и таблица кусочной функции.
 */

/**
 * @class TaskWindow
 * @brief Интерфейс для исследования кусочно-заданной функции с параметрами a, b, c.
 *
 * @details Функция определена как:
 * @code
 *   f(x) = { -x + a,      x < 0
 *           { x^b,         0 <= x < 2
 *           { x/2 + c,     x >= 2
 * @endcode
 * Три слайдера (a, b, c ∈ [−10, 10]) управляют параметрами в реальном времени.
 * QCustomPlot отображает три цветовых сегмента функции; QTableWidget —
 * таблицу значений для x ∈ {−10, −8, …, 10}.
 */
class TaskWindow : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Создаёт и компонует все виджеты окна задачи.
     * @param parent Родительский виджет (обычно nullptr).
     */
    explicit TaskWindow(QWidget* parent = nullptr);

    /**
     * @brief Задаёт имя вошедшего пользователя в заголовке окна.
     * @param name Отображаемое имя пользователя (из ответа сервера).
     */
    void setName(const QString& name);

signals:
    /**
     * @brief Испускается при нажатии кнопки «Выйти».
     *
     * @details В main.cpp этот сигнал подключён к QApplication::quit().
     */
    void logout();

private slots:
    /**
     * @brief Пересчитывает и отрисовывает график и таблицу по текущим значениям слайдеров.
     */
    void updatePlot();

private:
    QCustomPlot*  plot;        ///< Область отрисовки графика функции.
    QSlider*      sliderA;     ///< Слайдер параметра a (смещение при x < 0).
    QSlider*      sliderB;     ///< Слайдер параметра b (показатель степени при 0 ≤ x < 2).
    QSlider*      sliderC;     ///< Слайдер параметра c (смещение при x ≥ 2).

    QLabel*       labelA;      ///< Текстовое отображение текущего значения a.
    QLabel*       labelB;      ///< Текстовое отображение текущего значения b.
    QLabel*       labelC;      ///< Текстовое отображение текущего значения c.

    QPushButton*  btnLogout;   ///< Кнопка выхода из аккаунта.
    QTableWidget* table;       ///< Таблица значений: колонки X и F(x).

    /**
     * @brief Создаёт строку управления слайдером с подписью и меткой значения.
     *
     * @details Инициализирует слайдер с диапазоном [−10, 10] и подключает
     * его сигнал valueChanged к updatePlot().
     *
     * @param name       Название параметра (отображается слева от слайдера).
     * @param slider     Выходной указатель на созданный QSlider.
     * @param valueLabel Выходной указатель на метку текущего значения.
     * @return Готовый горизонтальный layout строки управления.
     */
    QHBoxLayout* makeSliderRow(const QString& name,
                                QSlider*& slider,
                                QLabel*& valueLabel);

    /**
     * @brief Вычисляет значение кусочной функции в точке x.
     *
     * @param x Аргумент функции.
     * @param a Параметр смещения для ветви x < 0.
     * @param b Показатель степени для ветви 0 ≤ x < 2.
     * @param c Параметр смещения для ветви x ≥ 2.
     * @return Значение f(x) согласно определению функции.
     */
    double f(double x, int a, int b, int c);

    /**
     * @brief Применяет стиль к QCustomPlot: тёмный фон, белые оси, сетка, легенда.
     */
    void setupPlot();

    /**
     * @brief Заполняет таблицу значениями функции для x ∈ {−10, −8, …, 10}.
     *
     * @param a Текущее значение параметра a.
     * @param b Текущее значение параметра b.
     * @param c Текущее значение параметра c.
     */
    void updateTable(int a, int b, int c);
};
