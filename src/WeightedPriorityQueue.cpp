#include <pthread.h>
#include <tuple>

#include "WeightedPriorityQueue.h"

using std::list;
using std::tuple;

pthread_mutex_t lock;

int main(int argc, char** argv) {
    return EXIT_SUCCESS;
}

list<Message> dequeueBatch(int batchSize) {
    list<Message> batch;
    pthread_mutex_lock(&lock);
    tuple<int, int, int> sizes = tuple(p1.size(), p2.size(), p3.size());
    tuple<int, int, int> weights = dm.getWeights(batchSize, sizes);
    for (int i = 0; i < get<0>(weights); i++) {
        if (!p1.empty()) {
            batch.add(p1.pop());
        } else {
            break;
        }
    }
    for (int i = 0; i < get<1>(weights); i++) {
        if (!p2.empty()) {
            batch.add(p2.pop());
        } else {
            break;
        }
    }
    for (int i = 0; i < get<2>(weights); i++) {
        if (!p3.empty()) {
            batch.add(p3.pop());
        } else {
            break;
        }
    }
    pthread_mutex_unlock(&lock);
    return batch;
}
