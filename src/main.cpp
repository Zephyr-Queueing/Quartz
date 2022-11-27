#include <iostream>
#include <thread>

#include "LoadGenerator.h"
#include "WeightedPriorityQueue.h"

#define LOAD_RATE 5.0
#define LOAD_WEIGHT_ONE 20.0
#define LOAD_WEIGHT_TWO 10.0
#define LOAD_WEIGHT_THREE 5.0

using namespace std;

int main(int argc, char** argv) {
    Standard standard;
    WeightedPriorityQueue wpq(standard);
    tuple<double, double, double> dist({ LOAD_WEIGHT_ONE, LOAD_WEIGHT_TWO, LOAD_WEIGHT_THREE });
    thread lg(LG(), LOAD_RATE, dist, ref(wpq));
    lg.join();
    cout << "WPQ Size: " << wpq.size() << endl;
}
