#ifndef WEIGHTED_PRIORITY_QUEUE_H
#define WEIGHTED_PRIORITY_QUEUE_H

#include <list>
#include <queue>
#include <Message.h>

#include "DistributionModel.h"

using std::list;
using std::queue;

class WeightedPriorityQueue {
  public:
    WeightedPriorityQueue(DistributionModel &m) : dm(m) {};
    
    void enqueue(const Message &msg);

    list<Message> dequeueBatch(int batchSize);

    int size();

  private:
    queue<Message> p1;
    queue<Message> p2;
    queue<Message> p3;
    DistributionModel &dm;

};

#endif // WEIGHTED_PRIORITY_QUEUE_H
