#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTimer>
#include <QThread>
#include <QMetaObject>
#include <QTime>
#include <QVBoxLayout>
#include <QChart>
#include <QChartView>
#include <cstdlib>

#include "asynctask.h"

// 🔹 Simulated heavy task with progress
void MainWindow::loadPatientHistory() {
    for (int i = 0; i <= 100; i += 10) {
        QThread::msleep(300);

        QMetaObject::invokeMethod(this, [this, i]() {
            ui->progressBar->setValue(i);
        });
    }
}

// 🔹 Status updater
void MainWindow::updateStatus() {
    QString text;

    switch (currentState) {
    case Idle:
        text = "Status: Idle";
        break;
    case Monitoring:
        text = "Status: Monitoring | " + QTime::currentTime().toString();
        break;
    case Loading:
        text = "Status: Loading patient history...";
        break;
    case Completed:
        text = "Status: Completed ✅";
        break;
    case Alert:
        text = "Status: ALERT ⚠ High Heart Rate";
        break;
    }

    ui->statusLabel->setText(text);
}

// 🔥 Constructor
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 🔹 Initial UI
    ui->progressBar->setValue(0);

    // =========================
    // 🔹 GRAPH SETUP
    // =========================
    series = new QLineSeries();

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Heart Rate Monitor");

    axisX = new QValueAxis();
    axisX->setRange(0, 30);
    axisX->setTitleText("Time");

    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(50, 120);
    axisY->setTitleText("BPM");

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);

    series->attachAxis(axisX);
    series->attachAxis(axisY);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    QVBoxLayout *layout = new QVBoxLayout(ui->chartContainer);
    layout->addWidget(chartView);

    // =========================
    // 🔹 TIMER (LIVE DATA)
    // =========================
    timer = new QTimer(this);

    connect(timer, &QTimer::timeout, this, [=]() mutable {

        int heartRate = 60 + rand() % 40;
        int oxygen = 90 + rand() % 10;
        float temp = 36.0 + (rand() % 30) / 10.0;

        // 🔹 Update labels
        ui->heartLabel->setText(
            QString("Heart Rate: %1 BPM").arg(heartRate)
            );
        ui->oxygenLabel->setText(
            QString("Oxygen: %1 %").arg(oxygen)
            );
        ui->tempLabel->setText(
            QString("Temperature: %1 °C").arg(temp)
            );

        // 🔹 Graph update
        series->append(timeCounter++, heartRate);
        if (timeCounter > 30) {
            axisX->setRange(timeCounter - 30, timeCounter);
        }

        // 🔹 Alert logic
        if (heartRate > 100) {
            currentState = Alert;
            ui->heartLabel->setStyleSheet("color: red;");
        } else {
            currentState = Monitoring;
            ui->heartLabel->setStyleSheet("color: green;");
        }

        updateStatus();
    });

    // =========================
    // 🔹 START BUTTON
    // =========================
    connect(ui->startButton, &QPushButton::clicked, this, [=]() {
        currentState = Monitoring;
        updateStatus();
        timer->start(1000);
    });


    connect(ui->loadButton, &QPushButton::clicked, this, [=]() {

        currentState = Loading;
        ui->progressBar->setValue(0);
        updateStatus();

                AsyncTask::run([this]() {

            loadPatientHistory();

            QMetaObject::invokeMethod(this, [this]() {
                currentState = Completed;
                updateStatus();

                QTimer::singleShot(2000, this, [this]() {
                    currentState = Monitoring;
                    updateStatus();
                });
            });
        });
}

MainWindow::~MainWindow()
{
    delete ui;
}
