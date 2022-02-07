//
// Created by Víctor Jiménez Rugama on 2/4/22.
//
#include "../../include/utils/threadPool.h"
#include "../../include/utils/logger.h"
#include "../../include/base.h"

bool threadPool::shutdown = false;
uint32_t threadPool::maxHardwareThreads = std::thread::hardware_concurrency();

std::mutex threadPool::queueMutex;
std::mutex threadPool::threadPoolMutex;
std::queue<std::function<void()>> threadPool::jobQueue;
std::vector<std::thread> threadPool::pool;

int threadPool::init() {
    if(!pool.empty())
        return 1;

    shutdown = false;
    for(int i=0; i < THREADPOOL_AMOUNT; i++)
        pool.emplace_back(std::thread(&threadPool::waitForTask));
    return 0;
}

int threadPool::term() {
    if(pool.empty())
        return 1;
    {
        std::unique_lock<std::mutex> lock(threadPoolMutex);
        shutdown = true; // use this flag in condition.wait
    }

    // Join all threads.
    for (std::thread &th : pool)
    {
        th.join();
    }

    pool.clear();
    return 0;
}

void threadPool::waitForTask() {
    while (!shutdown)
    {
        std::function<void()> job = nullptr;

        {
            std::unique_lock<std::mutex> lock(queueMutex);

            if(shutdown)
                break;
            else if(!jobQueue.empty()){
                job = jobQueue.front();
                jobQueue.pop();
            }

        }
        if(job != nullptr){
            job(); // function<void()> type
        }
    }
}

int threadPool::addJob(const std::function<void()>& newJob) {
    if(pool.empty())
        return -1;
    std::unique_lock<std::mutex> lock(queueMutex);
    jobQueue.emplace(newJob);
    return 0;
}

