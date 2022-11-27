#include <list>
#include <queue>
#include <"DistributionModel.h">

using std::list;
using std::queue;

typedef struct WeightedPriorityQueue {
    queue<Message> p1;
    queue<Message> p2;
    queue<Message> p3;
    DistributionModel dm;

    WeightedPriorityQueue(const DistributionModel m) : dm(m);

    list<Message> dequeueBatch(int batchSize);
} WeightedPriorityQueue;
