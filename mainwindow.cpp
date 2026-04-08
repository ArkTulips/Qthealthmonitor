#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QTimer>
#include <cstdlib>
#include <QPushButton>
#include <QThread>
#include <QMetaObject>

#include "asynctask.h"

// 🔹 Simulated heavy task
void MainWindow::loadPatientHistory() {
    QThread::sleep(5); // simulate heavy work
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 🔹 Button 1 (simple test)
    connect(ui->pushButton, &QPushButton::clicked, this, [=]() {
        ui->label->setText("Running...");
    });

    // 🔹 Live patient data (updates every second)
    QTimer *timer = new QTimer(this);

    connect(timer, &QTimer::timeout, this, [=]() {
        int heartRate = 60 + rand() % 40;
        int oxygen = 90 + rand() % 10;
        float temp = 36.0 + (rand() % 30) / 10.0;

        ui->label->setText("HR: " + QString::number(heartRate));
    });

    timer->start(1000);

    // 🔹 Button 2 (ASYNC VERSION - NO FREEZE)
    connect(ui->pushButton_2, &QPushButton::clicked, this, [=]() {
        ui->label->setText("Loading...");

        AsyncTask::run([this]() {
            loadPatientHistory();

            // update UI safely from main thread
            QMetaObject::invokeMethod(this, [this]() {
                ui->label->setText("Done (No Freeze)");
            });
        });
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}