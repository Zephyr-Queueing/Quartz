#include <pthread.h>
#include <tuple>
#include <Message.h>

#include "WeightedPriorityQueue.h"

using std::list;
using std::tuple;

pthread_mutex_t lock;

using namespace std;

int WeightedPriorityQueue::size() {
    pthread_mutex_lock(&lock);
    int tmp = p1.size() + p2.size() + p3.size();
    pthread_mutex_unlock(&lock);
    return tmp;
}

void WeightedPriorityQueue::enqueue(const Message &msg) {
    pthread_mutex_lock(&lock);

    switch(msg.priority) {
        case 1:
            p1.push(msg);
            dm.notify(1, 1);
            break;
        case 2:
            p2.push(msg);
            dm.notify(2, 1);
            break;
        case 3:
            p3.push(msg);
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

    tuple<int, int, int> sizes(p1.size(), p2.size(), p3.size());
    tuple<double, double, double> weights = dm.getWeights(sizes);

    for (int i = 0; i < get<0>(weights) * batchSize; i++) {
        if (p1.empty()) break;
        Message &msg = p1.front();
        p1.pop();
        batch.push_back(msg);
        dm.notify(1, -1);
    }
    for (int i = 0; i < get<1>(weights) * batchSize; i++) {
        if (p2.empty()) break;
        Message &msg = p2.front();
        p2.pop();
        batch.push_back(msg);
        dm.notify(2, -1);
    }
    for (int i = 0; i < get<2>(weights) * batchSize; i++) {
        if (p3.empty()) break;
        Message &msg = p3.front();
        p3.pop();
        batch.push_back(msg);
        dm.notify(3, -1);
    }

    pthread_mutex_unlock(&lock);
    return batch;
}
