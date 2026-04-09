#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineSeries>
#include <QValueAxis>
#include <QChart>
#include <QChartView>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE


// 🔹 App states
enum AppState {
    Idle,
    Monitoring,
    Loading,
    Completed,
    Alert
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    Ui::MainWindow *ui;

    // 🔹 State
    AppState currentState = Idle;

    // 🔹 Graph
    QLineSeries *series;
    QValueAxis *axisX;
    int timeCounter = 0;

    // 🔹 Timer
    QTimer *timer;

    // 🔹 Methods
    void updateStatus();
    void loadPatientHistory();
};

#endif