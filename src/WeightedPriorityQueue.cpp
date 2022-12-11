#include <pthread.h>
#include <tuple>
#include <Message.h>

#include "WeightedPriorityQueue.h"

using std::list;
using std::tuple;

pthread_mutex_t lock;

using namespace std;

tuple<int, int, int> WeightedPriorityQueue::sizes() {
    pthread_mutex_lock(&lock);
    tuple<int, int, int> tmp(p1.size(), p2.size(), p3.size());
    pthread_mutex_unlock(&lock);
    return tmp;
}

tuple<double, double, double> WeightedPriorityQueue::weights() {
    return dm.getWeights(sizes());
}

void WeightedPriorityQueue::enqueueFront(const Message &msg) {
    pthread_mutex_lock(&lock);

    switch(msg.priority) {
        case 1:
            p1.push_front(msg);
            dm.notify(1, 1);
            break;
        case 2:
            p2.push_front(msg);
            dm.notify(2, 1);
            break;
        case 3:
            p3.push_front(msg);
            dm.notify(3, 1);
            break;
        default:
            // Handle Invalid Message
            break;
    }

    pthread_mutex_unlock(&lock);
}

void WeightedPriorityQueue::enqueue(const Message &msg) {
    pthread_mutex_lock(&lock);

    switch(msg.priority) {
        case 1:
            p1.push_back(msg);
            dm.notify(1, 1);
            break;
        case 2:
            p2.push_back(msg);
            dm.notify(2, 1);
            break;
        case 3:
            p3.push_back(msg);
            dm.notify(3, 1);
            break;
        default:
            // Handle Invalid Message
            break;
    }

    pthread_mutex_unlock(&lock);
}

list<Message> WeightedPriorityQueue::dequeueBatch(int batchSize) {
    list<Message> batch;
    pthread_mutex_lock(&lock);

    chrono::milliseconds batchDequeueTime = chrono::duration_cast<chrono::milliseconds>(
         chrono::system_clock::now().time_since_epoch());

    tuple<int, int, int> sizes(p1.size(), p2.size(), p3.size());
    tuple<double, double, double> weights = dm.getWeights(sizes);

    // Remove from first priority
    for (int i = 0; i < get<0>(weights) * batchSize && !(p1.empty()); i++) {
        Message &msg = p1.front();
        p1.pop_front();
        batch.push_back(msg);
        dm.notify(1, -1);
    }

    // Remove from second priority
    for (int i = 0; i < get<1>(weights) * batchSize && !(p2.empty()); i++) {
        Message &msg = p2.front();
        p2.pop_front();
        batch.push_back(msg);
        dm.notify(2, -1);
    }

    // Remove from third priority
    for (int i = 0; i < get<2>(weights) * batchSize && !(p3.empty()); i++) {
        Message &msg = p3.front();
        p3.pop_front();
        batch.push_back(msg);
        dm.notify(3, -1);
    }

    for (Message &msg : batch) {
        msg.dequeueTime = batchDequeueTime;
    }

    pthread_mutex_unlock(&lock);
    return batch;
}
