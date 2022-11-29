#include <iostream>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <queue>
#include <thread>
#include <functional>

#include <ThreadPool.h>

using namespace std;

void ThreadPool::Start() {
    const uint32_t num_threads = std::thread::hardware_concurrency();
    threads.resize(num_threads);
    for (uint32_t i = 0; i < num_threads; i++) {
        threads.at(i) = thread(&ThreadLoop, this);
    }
}

void ThreadPool::ThreadLoop(ThreadPool *threadPool) {
    while (true) {
        function<void()> job;
        {
            unique_lock<mutex> lock(threadPool->queue_mutex);
            // atomically checks mutex_condition until it's notified, then loops until lambda returns true
            threadPool->mutex_condition.wait(lock, [threadPool] {
                return !threadPool->jobs.empty() || threadPool->should_terminate;
            });
            if (threadPool->should_terminate) {
                return;
            }
            job = threadPool->jobs.front();
            threadPool->jobs.pop();
        }
        job();
    }
}

void ThreadPool::QueueJob(const function<void()>& job) {
    {
        unique_lock<std::mutex> lock(queue_mutex);
        jobs.push(job);
    }
    mutex_condition.notify_one();
}

bool ThreadPool::Busy() {
    bool poolbusy;
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        poolbusy = !jobs.empty();
    }
    return poolbusy;
}

ThreadPool::~ThreadPool() {
    {
        unique_lock<std::mutex> lock(queue_mutex);
        should_terminate = true;
    }
    mutex_condition.notify_all();
    for (thread &active_thread : threads) {
        active_thread.join();
    }
    threads.clear();
}
