#include "taskwindow.h"
#include <cmath>

using namespace std;

/**

- @file taskwindow.cpp
- @brief Реализация формы задачи №1 с интерактивным графиком.
  */

TaskWindow::TaskWindow(QWidget* parent) : QWidget(parent) {
setWindowTitle("Задача 1 — Линейная кусочная функция");
setMinimumSize(700, 500);
// ── График ───────────────────────────────────────────────────────────────
plot = new QCustomPlot(this);
setupPlot();

// ── Ползунки ─────────────────────────────────────────────────────────────
QVBoxLayout* slidersLayout = new QVBoxLayout();
slidersLayout->addLayout(makeSliderRow("a", "f(x) = -x + a  (x < 0)",    sliderA, labelA));
slidersLayout->addLayout(makeSliderRow("b", "f(x) = x^b     (0 ≤ x < 2)", sliderB, labelB));
slidersLayout->addLayout(makeSliderRow("c", "f(x) = x/2 + c (x ≥ 2)",    sliderC, labelC));

// ── Кнопка отправки ──────────────────────────────────────────────────────
btnSubmit = new QPushButton("Отправить ответ на сервер");
connect(btnSubmit, &QPushButton::clicked, this, &TaskWindow::submitAnswer);

// ── Общая компоновка ─────────────────────────────────────────────────────
QVBoxLayout* rightLayout = new QVBoxLayout();
rightLayout->addLayout(slidersLayout);
rightLayout->addStretch();
rightLayout->addWidget(btnSubmit);

QHBoxLayout* mainLayout = new QHBoxLayout(this);
mainLayout->addWidget(plot, 3);       // график занимает 3/4 ширины
mainLayout->addLayout(rightLayout, 1); // ползунки — 1/4

// Первичная отрисовка
updatePlot();

}

void TaskWindow::setupPlot() {
plot->setBackground(QBrush(QColor(30, 30, 30)));
plot->xAxis->setBasePen(QPen(Qt::white));
plot->yAxis->setBasePen(QPen(Qt::white));
plot->xAxis->setTickPen(QPen(Qt::white));
plot->yAxis->setTickPen(QPen(Qt::white));
plot->xAxis->setTickLabelColor(Qt::white);
plot->yAxis->setTickLabelColor(Qt::white);
plot->xAxis->setLabelColor(Qt::white);
plot->yAxis->setLabelColor(Qt::white);
plot->xAxis->setLabel("x");
plot->yAxis->setLabel("f(x)");
// Сетка
plot->xAxis->grid()->setPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
plot->yAxis->grid()->setPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
plot->xAxis->grid()->setZeroLinePen(QPen(Qt::white, 1));
plot->yAxis->grid()->setZeroLinePen(QPen(Qt::white, 1));

// Три кривые для трёх участков функции
plot->addGraph(); // x < 0       — синий
plot->addGraph(); // 0 <= x < 2  — зелёный
plot->addGraph(); // x >= 2      — красный

plot->graph(0)->setPen(QPen(QColor(100, 180, 255), 2));
plot->graph(1)->setPen(QPen(QColor(100, 255, 150), 2));
plot->graph(2)->setPen(QPen(QColor(255, 120, 100), 2));

plot->xAxis->setRange(-10, 10);
plot->yAxis->setRange(-20, 20);

}

QHBoxLayout* TaskWindow::makeSliderRow(const QString& name,
const QString& formula,
QSlider*& slider,
QLabel*& valueLabel) {
QLabel* nameLabel    = new QLabel(QString("<b>%1</b>").arg(name));
QLabel* formulaLabel = new QLabel(formula);
valueLabel           = new QLabel("1");
nameLabel->setMinimumWidth(20);
valueLabel->setMinimumWidth(30);
valueLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

slider = new QSlider(Qt::Horizontal);
slider->setRange(-10, 10);
slider->setValue(1);
slider->setTickInterval(1);

connect(slider, &QSlider::valueChanged, this, &TaskWindow::updatePlot);

QHBoxLayout* row = new QHBoxLayout();
row->addWidget(nameLabel);
row->addWidget(formulaLabel);
row->addWidget(slider);
row->addWidget(valueLabel);
return row;

}

double TaskWindow::f(double x, int a, int b, int c) {
if (x < 0) {
return -x + a;
} else if (x < 2) {
return pow(x, b);
} else {
return x / 2.0 + c;
}
}

void TaskWindow::updatePlot() {
int a = sliderA->value();
int b = sliderB->value();
int c = sliderC->value();
// Обновляем подписи ползунков
labelA->setText(QString::number(a));
labelB->setText(QString::number(b));
labelC->setText(QString::number(c));

// Генерируем точки для каждого участка
QVector<double> x0, y0; // x < 0
QVector<double> x1, y1; // 0 <= x < 2
QVector<double> x2, y2; // x >= 2

int N = 500;

// Участок 1: x от -10 до 0
for (int i = 0; i <= N; ++i) {
    double x = -10.0 + (10.0 / N) * i;
    x0.append(x);
    y0.append(f(x, a, b, c));
}

// Участок 2: x от 0 до 2
for (int i = 0; i <= N; ++i) {
    double x = (2.0 / N) * i;
    x1.append(x);
    y1.append(f(x, a, b, c));
}

// Участок 3: x от 2 до 10
for (int i = 0; i <= N; ++i) {
    double x = 2.0 + (8.0 / N) * i;
    x2.append(x);
    y2.append(f(x, a, b, c));
}

plot->graph(0)->setData(x0, y0);
plot->graph(1)->setData(x1, y1);
plot->graph(2)->setData(x2, y2);

plot->replot();

}

void TaskWindow::submitAnswer() {
    // Формируем ответ как “a,b,c”
    QString answer = QString("%1,%2,%3"")
    .arg(sliderA->value())
    .arg(sliderB->value())
    .arg(sliderC->value());
    emit answerSubmitted(1, 1, answer);

}