#include <iostream>
#include <thread>
#include <chrono>

#include "LoadGenerator.h"
#include "WeightedPriorityQueue.h"
#include "ThreadPool.h"
#include "Server.h"

#define SERVER_PORT 49842
#define LOAD_RATE 5.0
#define LOAD_WEIGHT_ONE 20.0
#define LOAD_WEIGHT_TWO 10.0
#define LOAD_WEIGHT_THREE 5.0

using namespace std;

int main(int argc, char** argv) {
    ThreadPool threadPool;
    threadPool.QueueJob([] {
        cout << "J1" << endl;
    });

    threadPool.QueueJob([] {
        cout << "J2" << endl;
    });

    threadPool.QueueJob([] {
        cout << "J2" << endl;
    });

    threadPool.QueueJob(Server(SERVER_PORT));

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
