#include <iostream>
#include <thread>
#include <chrono>
#include <string.h>

#include "LoadGenerator.h"
#include "WeightedPriorityQueue.h"
#include "ThreadPool.h"
#include "Server.h"

#define NUM_SERVERS 16
#define QUARTZ_LIFESPAN 60 * 60 * 24

#define LOAD_RATE 0.3
#define LOAD_WEIGHT_ONE 0.7
#define LOAD_WEIGHT_TWO 0.2
#define LOAD_WEIGHT_THREE 0.1

using namespace std;

static void ParseArgs(int argc, char** argv,
                      double *loadRate, double *loadWeight1,
                      double *loadWeight2, double *loadWeight3);
static void Usage(char *cmd);
static void PrintStats();

int main(int argc, char** argv) {
    double loadRate, loadWeight1, loadWeight2, loadWeight3;
    ParseArgs(argc, argv, &loadRate, &loadWeight1, &loadWeight2, &loadWeight3); 

    ThreadPool threadPool;
    // Standard standard;
    // WeightedPriorityQueue wpq(standard);
    Zephyr zephyr;
    WeightedPriorityQueue wpq(zephyr);
    tuple<double, double, double> loadDist(loadWeight1, loadWeight2, loadWeight3);   

    threadPool.QueueJob(LG(loadRate, loadDist, wpq));
    for (int i = 0; i < NUM_SERVERS; i++)
        threadPool.QueueJob(Server(wpq));
    threadPool.Start();

    PrintStats();    
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

static void PrintStats() {
    this_thread::sleep_for(chrono::seconds(QUARTZ_LIFESPAN));
    // TODO
}
