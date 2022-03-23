//
// Created by Víctor Jiménez Rugama on 2/4/22.
//

#ifndef INC_4PT_THREADPOOL_H
#define INC_4PT_THREADPOOL_H

#include <pthread.h>
#include <vector>
#include <mutex>
#include <queue>

class threadPool {
private:
    typedef void * (*THREADFUNCPTR)(void *);
    static bool shutdown;

    static std::mutex queueMutex;
    static std::deque<std::function<void()>> jobQueue;
    static std::vector<pthread_t> pool;

    static std::mutex queueMutexSecondary;
    static std::deque<std::function<void()>> jobQueueSecondary;
    static std::vector<pthread_t> secondaryPool;
public:
    static int init();
    static int term();

    static int addJob(const std::function<void()>& newJob, bool front = false);
    static int addJobSecondary(const std::function<void()>& newJob, bool front = false);

    static void waitForTask();
    static void waitForTaskSecondary();
};
#endif //INC_4PT_THREADPOOL_H
