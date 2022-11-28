#include <iostream>
#include <thread>
#include <chrono>

#include "LoadGenerator.h"
#include "WeightedPriorityQueue.h"
#include "ThreadPool.h"
#include "Server.h"

#define SERVER_SERVICE "51710"
#define LOAD_RATE 5.0
#define LOAD_WEIGHT_ONE 0.7
#define LOAD_WEIGHT_TWO 0.2
#define LOAD_WEIGHT_THREE 0.1

using namespace std;

int main(int argc, char** argv) {
    ThreadPool threadPool;
    Standard standard;
    WeightedPriorityQueue wpq(standard);
    tuple<double, double, double> loadDist({ LOAD_WEIGHT_ONE, LOAD_WEIGHT_TWO, LOAD_WEIGHT_THREE });    

    threadPool.QueueJob(LG(LOAD_RATE, loadDist, ref(wpq)));
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
