#include <tuple>
#include <vector>
#include <map>
#include <chrono>
#include <thread>
#include <assert.h>

#include <DistributionModel.h>

#define MAX_WINDOW 1000 // ms
#define MAX_Q_DELAY 500 // ms

using std::tuple;
using chrono::milliseconds;

using namespace std;

// Standard

tuple<double, double, double> Standard::getWeights(tuple<int, int, int> sizes) {
    return tuple<double, double, double>(dw1, dw2, dw3);
}

void Standard::notify(int qid, int delta) {}

// Zephyr

static int getQueueSize(int qid, tuple<int, int, int> &sizes) {
    switch (qid) {
        case 1:
            return get<0>(sizes);
        case 2:
            return get<1>(sizes);
        case 3:
            return get<2>(sizes);
        default:
            return -1;
    }
}

static double getQueueWeight(int qid) {
    switch (qid) {
        case 1:
            return DW1;
        case 2:
            return DW2;
        case 3:
            return DW3;
        default:
            return -1.0;
    }
}

double Zephyr::estimateSpareWeight(int qid, tuple<int, int, int> &sizes) {
    chrono::milliseconds now = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch());
    tuple<int, int, int> &ms = movingSums[qid];
    int64_t win = (now - get<0>(history[qid][get<0>(ms)])).count();
    assert(win <= MAX_WINDOW);
    double enqueueRate = (double) get<1>(ms) / win;
    double dequeueRate = (double) get<2>(ms) / win;
    double netThroughput = dequeueRate - enqueueRate;
    double drainDelay = getQueueSize(qid, sizes) / netThroughput;
    if (netThroughput <= 0 || drainDelay > MAX_Q_DELAY) { // stressed
        return 0.0;
    }
    // drainRate > 0 && drainDelay <= MAX_Q_DELAY -> can rebalance weights s.t. drainRate becomes 0
    
    // enqueueRate * spareWeight = netThroughput => netThroughput = 0
    return netThroughput / enqueueRate;
}

Zephyr::Zephyr() {
    for (int i = 1; i < 4; i++) {
        movingSums[i] = tuple<int, int, int>(0, 0, 0);
        history[i] = vector<tuple<milliseconds, int>>();
        history[i].push_back(tuple<milliseconds, int>(milliseconds(0), 0));
    }
}

tuple<double, double, double> Zephyr::getWeights(tuple<int, int, int> sizes) {
    double o1Spare = estimateSpareWeight(1, sizes);
    double o2Spare = estimateSpareWeight(2, sizes);
    double o3Spare = estimateSpareWeight(3, sizes);
    bool o1 = o1Spare == 0.0;
    bool o2 = o2Spare == 0.0;
    bool o3 = o3Spare == 0.0;
    int numStressed = int(o1) + int(o2) + int(o3);
    switch(numStressed) {
        case 0:
            return tuple<double, double, double>(dw1, dw2, dw3);
        case 1:
            // TODO
            return tuple<double, double, double>(dw1, dw2, dw3);
        case 2:
            // TODO
            return tuple<double, double, double>(dw1, dw2, dw3);
        case 3:
            // TODO
            return tuple<double, double, double>(dw1, dw2, dw3);
        default:
            // TODO
            return tuple<double, double, double>(dw1, dw2, dw3);
    }
}

void Zephyr::notify(int qid, int delta) {
    chrono::milliseconds now = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch());
    chrono::milliseconds low = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()) - milliseconds(MAX_WINDOW);
    vector<tuple<milliseconds, int>> &qhist = history[qid];
    qhist.push_back(tuple<milliseconds, int>(now, delta));
    tuple<int, int, int> &p = movingSums[qid];
    int ndelta;
    while (get<0>(history[qid][get<0>(p)]) < low) {
        ndelta = get<1>(history[qid][++(get<0>(p))]);
        if (ndelta > 0) // enqueue
            get<1>(p) -= ndelta;
        else            // dequeue
            get<2>(p) -= (-1) * ndelta;
    }
}