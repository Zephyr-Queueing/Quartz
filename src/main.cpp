#include <iostream>

#include "WeightedPriorityQueue.h"

using namespace std;

int main(int argc, char** argv) {
    Standard standard;
    WeightedPriorityQueue wpq(standard);
    Message dummyMsg = { 1, string("mydata"), milliseconds(1000), milliseconds(1000) };
    wpq.enqueue(dummyMsg);
    wpq.enqueue(dummyMsg);
    wpq.enqueue(dummyMsg);

    cout << "WPQ Size: " << wpq.size() << endl;
}
