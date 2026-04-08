#ifndef ASYNCTASK_H
#define ASYNCTASK_H

#include <functional>

class AsyncTask {
public:
    static void run(std::function<void()> task);
};

#endif