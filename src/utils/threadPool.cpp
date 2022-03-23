//
// Created by Víctor Jiménez Rugama on 2/4/22.
//
#include <orbis/libkernel.h>
#include <thread>
#include "../../include/utils/threadPool.h"
#include "../../include/base.h"
#include "../../include/utils/logger.h"

//TODO TRY GENERATING THREADS AT THAT MOMENT AND THEN IF MAX NUMBER REACHED WAIT
bool threadPool::shutdown = true;

std::mutex threadPool::queueMutex;
std::deque<std::function<void()>> threadPool::jobQueue;
std::vector<pthread_t> threadPool::pool;

std::mutex threadPool::queueMutexSecondary;
std::deque<std::function<void()>> threadPool::jobQueueSecondary;
std::vector<pthread_t> threadPool::secondaryPool;

int threadPool::init() {
    shutdown = false;
    pthread_t newThread;
    int ret;
    for(int i=0; i < THREADPOOL_MAIN_POOL_AMOUNT; i++) {
        ret = pthread_create(&newThread, NULL, (THREADFUNCPTR) &threadPool::waitForTask,NULL);
        if (ret){
            LOG << "Error:unable to create thread " << ret;
        }
        pool.emplace_back(newThread);
    }

    for(int i=0; i < THREADPOOL_SECONDARY_POOL_AMOUNT; i++) {
        ret = pthread_create(&newThread, NULL, (THREADFUNCPTR) &threadPool::waitForTaskSecondary,NULL);
        if (ret){
            LOG << "Error:unable to create thread " << ret;
        }
        secondaryPool.emplace_back(newThread);
    }
    return 0;
}

int threadPool::term() {
    if(pool.empty() && secondaryPool.empty())
        return 0;

    shutdown = true; // use this flag in condition.wait
    int ret;
    // Join all threads.
    for (pthread_t threadID : pool)
    {
        ret = pthread_join(threadID, NULL);
        if (ret) {
            LOG << "Error when joining " << threadID;
        }
    }

    // Join all threads.
    for (pthread_t threadID : secondaryPool)
    {
        ret = pthread_join(threadID, NULL);
        if (ret) {
            LOG << "Error when joining " << threadID;
        }
    }

    pool.clear();
    secondaryPool.clear();
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
                jobQueue.pop_front();
            }

        }
        if(job != nullptr){
            job(); // function<void()> type
        } else {
            std::this_thread::yield();
        }
    }
}


void threadPool::waitForTaskSecondary() {
    while (!shutdown)
    {
        std::function<void()> job = nullptr;

        {
            std::lock_guard<std::mutex> lock(queueMutexSecondary);

            if(shutdown)
                break;
            else if(!jobQueueSecondary.empty()){
                job = jobQueueSecondary.front();
                jobQueueSecondary.pop_front();
            }

        }
        if(job != nullptr){
            job(); // function<void()> type
        } else {
            std::this_thread::yield();
        }
    }
}

int threadPool::addJob(const std::function<void()>& newJob, bool front) {
    if(shutdown || pool.empty())
        return -1;
    std::lock_guard<std::mutex> lock(queueMutex);
    if(front)
        jobQueue.emplace_front(newJob);
    else
        jobQueue.emplace_back(newJob);
    return 0;
}

int threadPool::addJobSecondary(const std::function<void()>& newJob, bool front) {
    if(shutdown || secondaryPool.empty())
        return -1;
    std::lock_guard<std::mutex> lock(queueMutexSecondary);
    if(front)
        jobQueueSecondary.emplace_front(newJob);
    else
        jobQueueSecondary.emplace_back(newJob);
    return 0;
}
