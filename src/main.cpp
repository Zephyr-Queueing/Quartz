#include <iostream>
#include <thread>
#include <chrono>

#include "LoadGenerator.h"
#include "WeightedPriorityQueue.h"
#include "ThreadPool.h"
#include "Server.h"

#define SERVER_SERVICE "51710"
#define LOAD_RATE 5.0
#define LOAD_WEIGHT_ONE 20.0
#define LOAD_WEIGHT_TWO 10.0
#define LOAD_WEIGHT_THREE 5.0

using namespace std;

int main(int argc, char** argv) {
    ThreadPool threadPool;
    Standard standard;
    WeightedPriorityQueue wpq(standard);
    tuple<double, double, double> dist({ LOAD_WEIGHT_ONE, LOAD_WEIGHT_TWO, LOAD_WEIGHT_THREE });    

    threadPool.QueueJob(LG(LOAD_RATE, dist, ref(wpq)));
    threadPool.QueueJob(Server(SERVER_SERVICE, ref(wpq)));
    threadPool.QueueJob(Server(SERVER_SERVICE, ref(wpq)));

    threadPool.Start();

    this_thread::sleep_for(chrono::seconds(60 * 10));

    threadPool.Stop();
    

    /*
    Standard standard;
    WeightedPriorityQueue wpq(standard);
    tuple<double, double, double> dist({ LOAD_WEIGHT_ONE, LOAD_WEIGHT_TWO, LOAD_WEIGHT_THREE });
    thread lg(LG(), LOAD_RATE, dist, ref(wpq));
    lg.join();
    cout << "WPQ Size: " << wpq.size() << endl;
    */
}
