#include <mutex>
#include <condition_variable>
#include <vector>
#include <queue>
#include <thread>
#include <functional>

using namespace std;

class ThreadPool {
  public:
    void Start();
    void QueueJob(const function<void()>& job);
    void Stop();
    bool Busy();

  private:
    static void ThreadLoop(ThreadPool *threadPool);

    bool should_terminate = false;
    mutex queue_mutex;
    condition_variable mutex_condition;
    vector<thread> threads;
    queue<function<void()>> jobs;
};