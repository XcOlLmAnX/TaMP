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

class TaskWindow : public QWidget {
    Q_OBJECT

public:
    explicit TaskWindow(QWidget* parent = nullptr);
    void setName(const QString& name);

    signals:
        void logout();

private slots:
    void updatePlot();

private:
    QCustomPlot*  plot;
    QSlider*      sliderA;
    QSlider*      sliderB;
    QSlider*      sliderC;
    QLabel*       labelA;
    QLabel*       labelB;
    QLabel*       labelC;
    QPushButton*  btnLogout;
    QTableWidget* table;

    QHBoxLayout* makeSliderRow(const QString& name,
                                QSlider*& slider,
                                QLabel*& valueLabel);

    double f(double x, int a, int b, int c);
    void setupPlot();
    void updateTable(int a, int b, int c);
};