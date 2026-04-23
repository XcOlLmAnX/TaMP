#include "taskwindow.h"
#include "tcpclient.h"
#include <cmath>

using namespace std;

TaskWindow::TaskWindow(QWidget* parent) : QWidget(parent) {
    setMinimumSize(900, 600);

    plot = new QCustomPlot(this);
    setupPlot();

    QLabel* taskTitle = new QLabel("<b>Численная задача</b>");
    taskTitle->setAlignment(Qt::AlignCenter);
    taskTitle->setStyleSheet("font-size: 14px; ");
    QLabel* formulaLabel = new QLabel;
    QPixmap formulaPixmap("formula.jpg");
    if (!formulaPixmap.isNull()) {
        formulaLabel->setPixmap(formulaPixmap.scaledToWidth(265, Qt::SmoothTransformation));
    } else {
        formulaLabel->setText("formula.jpg не найден");
    }
    formulaLabel->setAlignment(Qt::AlignCenter);
    formulaLabel->setMinimumWidth(265);
    formulaLabel->setMinimumHeight(150);

    QLabel* slidersTitle = new QLabel("<b>Настройка параметров</b>");

    QVBoxLayout* slidersLayout = new QVBoxLayout();
    slidersLayout->addWidget(slidersTitle);
    slidersLayout->addLayout(makeSliderRow("a", sliderA, labelA));
    slidersLayout->addLayout(makeSliderRow("b", sliderB, labelB));
    slidersLayout->addLayout(makeSliderRow("c", sliderC, labelC));

    table = new QTableWidget(this);
    table->setColumnCount(2);
    table->setHorizontalHeaderLabels({"X", "F(x)"});
    table->horizontalHeader()->setStretchLastSection(true);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setFixedWidth(300);

    btnLogout = new QPushButton("Выйти");
    connect(btnLogout, &QPushButton::clicked, this, &TaskWindow::logout);

    QVBoxLayout* leftLayout = new QVBoxLayout();
    leftLayout->addWidget(taskTitle);
    leftLayout->addWidget(formulaLabel);
    leftLayout->addSpacing(10);
    leftLayout->addLayout(slidersLayout);
    leftLayout->addSpacing(10);
    leftLayout->addWidget(table, 1);
    leftLayout->addSpacing(10);
    leftLayout->addWidget(btnLogout);

    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->addLayout(leftLayout, 1);
    mainLayout->addWidget(plot, 3);

    updatePlot();
}

void TaskWindow::setName(const QString& name) {
    setWindowTitle(QString(" [%1]").arg(name));
}

void TaskWindow::setupPlot() {
    plot->setBackground(QBrush(QColor(28, 28, 28)));
    plot->xAxis->setBasePen(QPen(Qt::white));
    plot->yAxis->setBasePen(QPen(Qt::white));
    plot->xAxis->setTickPen(QPen(Qt::white));
    plot->yAxis->setTickPen(QPen(Qt::white));
    plot->xAxis->setTickLabelColor(Qt::white);
    plot->yAxis->setTickLabelColor(Qt::white);
    plot->xAxis->setLabel("x");
    plot->yAxis->setLabel("f(x)");
    plot->xAxis->setLabelColor(Qt::white);
    plot->yAxis->setLabelColor(Qt::white);
    plot->xAxis->grid()->setPen(QPen(QColor(70, 70, 70), 1, Qt::DotLine));
    plot->yAxis->grid()->setPen(QPen(QColor(70, 70, 70), 1, Qt::DotLine));
    plot->xAxis->grid()->setZeroLinePen(QPen(QColor(150, 150, 150), 1));
    plot->yAxis->grid()->setZeroLinePen(QPen(QColor(150, 150, 150), 1));

    plot->addGraph();
    plot->addGraph();
    plot->addGraph();

    plot->graph(0)->setPen(QPen(QColor(100, 180, 255), 2));
    plot->graph(1)->setPen(QPen(QColor(100, 230, 130), 2));
    plot->graph(2)->setPen(QPen(QColor(255, 120, 100), 2));

    plot->graph(0)->setName("x < 0");
    plot->graph(1)->setName("0 ≤ x < 2");
    plot->graph(2)->setName("x ≥ 2");

    plot->legend->setVisible(true);
    plot->legend->setBrush(QBrush(QColor(40, 40, 40)));
    plot->legend->setTextColor(Qt::white);
    plot->legend->setBorderPen(QPen(QColor(80, 80, 80)));

    plot->xAxis->setRange(-10, 10);
    plot->yAxis->setRange(-20, 20);
    plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
}

QHBoxLayout* TaskWindow::makeSliderRow(const QString& name,
                                        QSlider*& slider,
                                        QLabel*& valueLabel) {
    QLabel* nameLabel = new QLabel(QString("<b>%1</b>").arg(name));
    valueLabel        = new QLabel("1");
    nameLabel->setFixedWidth(16);
    valueLabel->setFixedWidth(28);
    valueLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    slider = new QSlider(Qt::Horizontal);
    slider->setRange(-10, 10);
    slider->setValue(0);
    slider->setTickInterval(1);
    slider->setTickPosition(QSlider::TicksBelow);
    connect(slider, &QSlider::valueChanged, this, &TaskWindow::updatePlot);

    QHBoxLayout* row = new QHBoxLayout();
    row->addWidget(nameLabel);
    row->addWidget(slider);
    row->addWidget(valueLabel);
    return row;
}

double TaskWindow::f(double x, int a, int b, int c) {
    if (x < 0)      return -x + a;
    else if (x < 2) return pow(x, b);
    else            return x / 2.0 + c;
}

void TaskWindow::updateTable(int a, int b, int c) {
    QVector<double> xs;
    for (int x = -10; x <= 10; x += 2)
        xs.append(x);

    table->setRowCount(xs.size());
    for (int i = 0; i < xs.size(); ++i) {
        double x  = xs[i];
        double fx = f(x, a, b, c);
        table->setItem(i, 0, new QTableWidgetItem(QString::number(x,  'f', 2)));
        table->setItem(i, 1, new QTableWidgetItem(QString::number(fx, 'f', 4)));
    }
}

void TaskWindow::updatePlot() {
    int a = sliderA->value();
    int b = sliderB->value();
    int c = sliderC->value();

    labelA->setText(QString::number(a));
    labelB->setText(QString::number(b));
    labelC->setText(QString::number(c));

    int N = 500;
    QVector<double> x0, y0, x1, y1, x2, y2;

    for (int i = 0; i <= N; ++i) {
        double x = -10.0 + (10.0 / N) * i;
        x0.append(x); y0.append(f(x, a, b, c));
    }
    for (int i = 0; i <= N; ++i) {
        double x = (2.0 / N) * i;
        x1.append(x); y1.append(f(x, a, b, c));
    }
    for (int i = 0; i <= N; ++i) {
        double x = 2.0 + (8.0 / N) * i;
        x2.append(x); y2.append(f(x, a, b, c));
    }

    plot->graph(0)->setData(x0, y0);
    plot->graph(1)->setData(x1, y1);
    plot->graph(2)->setData(x2, y2);
    plot->replot();

    updateTable(a, b, c);
}