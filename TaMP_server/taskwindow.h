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


class TaskWindow : public QWidget {
    Q_OBJECT

public:
    explicit TaskWindow(QWidget* parent = nullptr);

    signals:
    answerSubmitted(int taskNumber, int variant, const QString& answer);

private slots:
    void updatePlot();


private:
    QCustomPlot* plot;
    QSlider* sliderA;
    QSlider* sliderB;
    QSlider* sliderC;

    QLabel* labelA;
    QLabel* labelB;
    QLabel* labelC;



    QHBoxLayout* makeSliderRow(const QString& name,
                                  const QString& formula,
                                  QSlider*& slider,
                                  QLabel*& valueLabel);

    double f(double x, int a, int b, int c);

    void setupPlot();

};

#endif //TAMP_TASKWINDOW_H