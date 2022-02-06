//
// Created by Víctor Jiménez Rugama on 2/4/22.
//

#ifndef INC_4PT_THREADPOOL_H
#define INC_4PT_THREADPOOL_H

#include <thread>
#include <vector>
#include <mutex>
#include <queue>

class threadPool {
private:
    static bool shutdown;
    static std::mutex queueMutex;
    static std::mutex threadPoolMutex;
    static uint32_t maxHardwareThreads;
    static std::queue<std::function<void()>> jobQueue;
    static std::vector<std::thread> pool;
    static void waitForTask();
public:
    static int init();
    static int term();
    static int addJob(const std::function<void()>& newJob);

};
#endif //INC_4PT_THREADPOOL_H
