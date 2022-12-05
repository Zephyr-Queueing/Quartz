#ifndef WEIGHTED_PRIORITY_QUEUE_H
#define WEIGHTED_PRIORITY_QUEUE_H

#include <list>
#include <deque>
#include <Message.h>

#include "DistributionModel.h"

using std::list;
using std::deque;

class WeightedPriorityQueue {
  public:
    WeightedPriorityQueue(DistributionModel &m) : dm(m) {};
    
    void enqueue(const Message &msg);
    void enqueueFront(const Message &msg);

    list<Message> dequeueBatch(int batchSize);

    int size();

  private:
    deque<Message> p1;
    deque<Message> p2;
    deque<Message> p3;
    DistributionModel &dm;

};

#endif // WEIGHTED_PRIORITY_QUEUE_H
