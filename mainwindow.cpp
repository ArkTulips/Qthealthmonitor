#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTimer>
#include <QThread>
#include <QMetaObject>
#include <QTime>
#include <QVBoxLayout>
#include <QChart>
#include <QChartView>
#include <QValueAxis>
#include <QPen>
#include <cstdlib>

#include "asynctask.h"

// =========================
// 🔹 PATIENT HISTORY
// =========================
QString MainWindow::loadPatientHistory() {

    QString result;

    int totalEntries = globalTime / 5;
    if (totalEntries < 1) totalEntries = 1;

    for (int i = 0; i < totalEntries; i++) {

        QThread::msleep(80);

        int hr = 60 + rand() % 40;

        int timeSec = (i + 1) * 5;

        result += QString("Time %1 sec → HR: %2 BPM\n")
                      .arg(timeSec)
                      .arg(hr);

        QMetaObject::invokeMethod(this, [this, i, totalEntries]() {
            int progress = (i * 100) / totalEntries;
            ui->progressBar->setValue(progress);
        });
    }

    QMetaObject::invokeMethod(this, [this]() {
        ui->progressBar->setValue(100);
    });

    return result;
}

// =========================
// 🔹 STATUS
// =========================
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

// =========================
// 🔥 CONSTRUCTOR
// =========================
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // =========================
    // 🔹 STYLE
    // =========================
    this->setStyleSheet(R"(

    QMainWindow {
        background-color: #0D1117;
    }

    QLabel#heartLabel {
        font-size: 30px;
        font-weight: bold;
        color: #00FFAA;
    }

    QLabel#oxygenLabel {
        font-size: 22px;
        color: #4FC3F7;
    }

    QLabel#tempLabel {
        font-size: 22px;
        color: #FFB74D;
    }

    QLabel#statusLabel {
        font-size: 18px;
        font-weight: bold;
        color: #FFD54F;
    }

    QPushButton {
        background-color: #1F6FEB;
        color: white;
        border-radius: 10px;
        padding: 8px;
        font-size: 16px;
        font-weight: bold;
    }

    QTextEdit {
        background-color: #161B22;
        color: #00FFAA;
        font-family: Consolas;
        font-size: 14px;
        border-radius: 8px;
        padding: 6px;
    }

    QProgressBar::chunk {
        background-color: #2EA043;
    }

    )");

    ui->progressBar->setValue(0);

    // =========================
    // 🔹 GRAPH SETUP
    // =========================
    series = new QLineSeries();

    // Glow-like line
    QPen pen(QColor("#00FFAA"));
    pen.setWidth(3);
    series->setPen(pen);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setBackgroundBrush(QBrush(QColor("#0D1117")));

    axisX = new QValueAxis();
    axisX->setRange(0, 100);
    axisX->setVisible(false);

    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(50, 120);
    axisY->setVisible(false);

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

        static int slowCounter = 0;
        static double lastHR = 75;
        static int oxygen = 98;
        static float temp = 36.5;

        slowCounter++;
        globalTime++;

        // 🔹 Smooth HR generation
        // 🔹 base variation
        // 🔹 natural pull toward baseline (75)
        double baseline = 75;

        double drift = (baseline - lastHR) * 0.05;  // pulls it back

        double randomChange = (rand() % 5 - 2);

        double targetHR = lastHR + drift + randomChange;

        // 🔹 occasional spike
        if (rand() % 25 == 0) {   // ~4% chance
            targetHR += 15 + rand() % 10;  // spike up
        }

        // 🔹 rare high spike (alert scenario)
        if (rand() % 60 == 0) {   // ~1.5% chance
            targetHR += 25 + rand() % 15;
        }

        // 🔹 clamp to safe range
        if (targetHR < 55) targetHR = 55;
        if (targetHR > 130) targetHR = 130;


        double smoothHR = lastHR + (targetHR - lastHR) * 0.3;
        lastHR = smoothHR;
        if (targetHR > lastHR)
            smoothHR = lastHR + (targetHR - lastHR) * 0.4; // fast rise
        else
            smoothHR = lastHR + (targetHR - lastHR) * 0.15; // slow fall

        // 🔹 Graph update (ONLY ONCE ✔)
        series->append(timeCounter++, smoothHR);

        // limit points
        if (series->count() > 200) {
            series->remove(0);
        }

        // smooth scrolling
        axisX->setRange(timeCounter - 100, timeCounter);

        // 🔹 Slow updates for vitals
        if (slowCounter % 10 == 0) {
            oxygen = 95 + rand() % 5;
            temp += (rand() % 3 - 1) * 0.05;
        }

        // 🔹 UI update
        ui->heartLabel->setText(
            QString("Heart Rate: %1 BPM").arg((int)smoothHR)
            );

        ui->oxygenLabel->setText(
            QString("Oxygen: %1 %").arg(oxygen)
            );

        ui->tempLabel->setText(
            QString("Temperature: %1 °C").arg(temp, 0, 'f', 1)
            );

        // 🔹 Alert
        if (smoothHR > 100) {
            currentState = Alert;
            ui->heartLabel->setStyleSheet("color: red;");
        } else {
            currentState = Monitoring;
            ui->heartLabel->setStyleSheet("color: #00FFAA;");
        }

        updateStatus();
    });

    // =========================
    // 🔹 START BUTTON
    // =========================
    connect(ui->startButton, &QPushButton::clicked, this, [=]() {
        currentState = Monitoring;
        updateStatus();
        timer->start(100);  // fast = smooth
    });

    // =========================
    // 🔹 LOAD BUTTON
    // =========================
    connect(ui->loadButton, &QPushButton::clicked, this, [=]() {

        currentState = Loading;
        ui->progressBar->setValue(0);
        updateStatus();

       connect(ui->loadButton, &QPushButton::clicked, this, [=]() {

        currentState = Loading;
        ui->progressBar->setValue(0);
        updateStatus();

        ui->statusLabel->setText("Loading...");
        loadPatientHistory();
        ui->statusLabel->setText("Done");
    });
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}
