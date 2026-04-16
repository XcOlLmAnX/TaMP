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

- @file taskwindow.h
- @brief Форма задачи №1 — линейная кусочная функция с интерактивным графиком.
-
- Пользователь двигает ползунки для параметров a, b, c.
- График обновляется в реальном времени.
  */
  class TaskWindow : public QWidget {
      Q_OBJECT

    public:
      /**
      * @brief Конструктор формы задачи.
      * @param parent Родительский виджет.
      */
      explicit TaskWindow(QWidget* parent = nullptr);

      signals:
      /**
      * @brief Сигнал отправки ответа на сервер.
      * @param taskNumber Номер задачи.
      * @param variant    Вариант.
      * @param answer     Ответ пользователя.
      */
      void answerSubmitted(int taskNumber, int variant, const QString& answer);

  private slots:
  /// @brief Обнавляет график при изменении любого ползунка.
  void updatePlot();
      /// @brief Отправляет ответ на сервер.
      void submitAnswer();

  private:
      QCustomPlot* plot;   ///< Виджет графика
      QSlider* sliderA;    ///< Ползунок параметра a (для -x + a)
      QSlider* sliderB;    ///< Ползунок параметра b (для x^b)
      QSlider* sliderC;    ///< Ползунок параметра c (для x/2 + c)

      QLabel* labelA;      ///< Текущее значение a
      QLabel* labelB;      ///< Текущее значение b
      QLabel* labelC;      ///< Текущее значение c

      QPushButton* btnSubmit;  ///< Кнопка отправки ответа

      /// @brief Создаёт строку с ползунком и подписями.
      QHBoxLayout* makeSliderRow(const QString& name,
                                  const QString& formula,
                                  QSlider*& slider,
                                  QLabel*& valueLabel);

      /// @brief Вычисляет f(x) по текущим параметрам a, b, c.
      double f(double x, int a, int b, int c);

      /// @brief Инициализирует внешний вид графика.
      void setupPlot();

  };

#endif //TAMP_TASKWINDOW_H