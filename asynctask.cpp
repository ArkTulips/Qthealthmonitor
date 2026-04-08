#include "asynctask.h"
#include <QThread>

class WorkerThread : public QThread {
public:
    std::function<void()> task;

    void run() override {
        if (task) task();
    }
};

void AsyncTask::run(std::function<void()> task) {
    WorkerThread *thread = new WorkerThread();
    thread->task = task;

    QObject::connect(thread, &QThread::finished, thread, &QObject::deleteLater);
    thread->start();
}