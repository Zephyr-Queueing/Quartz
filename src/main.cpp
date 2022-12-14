#include <iostream>
#include <thread>
#include <chrono>
#include <string.h>

#include "LoadGenerator.h"
#include "WeightedPriorityQueue.h"
#include "ThreadPool.h"
#include "Server.h"

#define NUM_SERVERS 1
#define STATS_INTERVAL 500 // ms

#define LOAD_RATE 0.3
#define LOAD_WEIGHT_ONE 0.7
#define LOAD_WEIGHT_TWO 0.2
#define LOAD_WEIGHT_THREE 0.1

using namespace std;

static void ParseArgs(int argc, char** argv,
                      double *loadRate, double *loadWeight1,
                      double *loadWeight2, double *loadWeight3);
static void Usage(char *cmd);
static void PrintStats(WeightedPriorityQueue &wpq);

int main(int argc, char** argv) {
    double loadRate, loadWeight1, loadWeight2, loadWeight3;
    ParseArgs(argc, argv, &loadRate, &loadWeight1, &loadWeight2, &loadWeight3); 

    ThreadPool threadPool;

    // uncomment comment lines below to change to standard model, and vice versa
    Zephyr zephyr;
    WeightedPriorityQueue wpq(zephyr);
    // Standard standard;
    // WeightedPriorityQueue wpq(standard);

    tuple<double, double, double> loadDist(loadWeight1, loadWeight2, loadWeight3);   

    threadPool.QueueJob(LG(loadRate, loadDist, wpq));
    for (int i = 0; i < NUM_SERVERS; i++)
        threadPool.QueueJob(Server(wpq));
    threadPool.Start();

    PrintStats(wpq);
}

static void PrintStats(WeightedPriorityQueue &wpq) {
    for (;;) {
        this_thread::sleep_for(chrono::milliseconds(STATS_INTERVAL));
        chrono::milliseconds now = chrono::duration_cast<chrono::milliseconds>(
                                   chrono::system_clock::now().time_since_epoch());
        tuple<int, int, int> sizes = wpq.sizes();
        tuple<double, double, double> weights = wpq.weights();
        cout << now.count() << ","
             << get<0>(sizes) << "," << get<1>(sizes) << "," << get<2>(sizes) << ","
             << get<0>(weights) << "," << get<1>(weights) << "," << get<2>(weights) << endl;
    }
}

static void ParseArgs(int argc, char** argv,
                      double *loadRate, double *loadWeight1,
                      double *loadWeight2, double *loadWeight3) {
    if (argc != 9 ||
        strcmp("-r", argv[1]) != 0 ||
        strcmp("-lw1", argv[3]) != 0 ||
        strcmp("-lw2", argv[5]) != 0 ||
        strcmp("-lw3", argv[7]) != 0) {
            Usage(argv[0]);
    }

    try {
        *loadRate = stod(argv[2]);
        *loadWeight1 = stod(argv[4]);
        *loadWeight2 = stod(argv[6]);
        *loadWeight3 = stod(argv[8]);
    } catch (...) {
        Usage(argv[0]);
    }
}

static void Usage(char *cmd) {
    cout << cmd << " -r <decimal> -lw1 <decimal> -lw2 <decimal> -lw3 <decimal>" << endl;
    exit(EXIT_FAILURE);
}

