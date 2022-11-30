#include <iostream>
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

static int getQueueSize(int qid, tuple<int, int, int> &sizes);
static double getQueueWeight(int qid);

// Standard

tuple<double, double, double> Standard::getWeights(tuple<int, int, int> sizes) {
    return tuple<double, double, double>(dw1, dw2, dw3);
}

void Standard::notify(int qid, int delta) {}

// Zephyr

Zephyr::Zephyr() {
    captureNow();
    for (int i = 1; i <= 3; i++) {
        movingSums[i] = tuple<int, int, int>(0, 0, 0);
        history[i] = vector<tuple<milliseconds, int>>();
        history[i].push_back(tuple<milliseconds, int>(now, 0));
    }
}

tuple<double, double, double> Zephyr::getWeights(tuple<int, int, int> sizes) {
    captureNow();
    refreshAllMovingSums();
    double o1Spare = estimateSpareWeight(0, sizes);
    double o2Spare = estimateSpareWeight(1, sizes);
    double o3Spare = estimateSpareWeight(2, sizes);
    double totSpare = o1Spare + o2Spare + o3Spare;
    int numStressed = int(o1Spare > 0.0) + int(o2Spare > 0.0) + int(o3Spare > 0.0);
    tuple<double, double, double> dws(dw1, dw2, dw3);
    if (numStressed != 0) {
        get<0>(dws) += totSpare / 3.0 - o1Spare;
        get<1>(dws) += totSpare / 3.0 - o2Spare;
        get<2>(dws) += totSpare / 3.0 - o3Spare;
    }
    return dws;
}

void Zephyr::notify(int qid, int delta) {
    cout << "notify " << qid << " " << delta << endl;
    captureNow();
    vector<tuple<milliseconds, int>> &qhist = history[qid];
    tuple<int, int, int> &p = movingSums[qid];
    qhist.push_back(tuple<milliseconds, int>(now, delta));
    if (delta > 0) get<1>(p) += delta;
    else get<2>(p) -= delta;
}

// drainRate > 0 && drainDelay <= MAX_Q_DELAY -> can rebalance weights s.t. drainRate becomes 0
// enqueueRate * spareWeight = netThroughput => netThroughput = 0
double Zephyr::estimateSpareWeight(int qid, tuple<int, int, int> &sizes) {
    tuple<int, int, int> &ms = movingSums[qid];
    int64_t win = MAX_WINDOW; //(now - get<0>(history[qid][get<0>(ms)])).count();
    assert(win <= MAX_WINDOW);
    double enqueueRate = (double) get<1>(ms) / win;
    double dequeueRate = (double) get<2>(ms) / win;
    double netThroughput = dequeueRate - enqueueRate;
    double drainDelay = getQueueSize(qid, sizes) / netThroughput;
    if (netThroughput <= 0 || drainDelay > MAX_Q_DELAY) { // stressed
        return 0.0;
    }
    return netThroughput / enqueueRate;
}

void Zephyr::refreshAllMovingSums() {
    for (int i = 0; i <= 2; i++) {
        refreshQueueMovingSums(i);
    }
}

void Zephyr::refreshQueueMovingSums(int qid) {
    milliseconds low = now - milliseconds(MAX_WINDOW);
    tuple<int, int, int> &p = movingSums[qid];
    int ndelta;
    while (get<0>(p) < history[qid].size() - 1 && get<0>(history[qid][get<0>(p)]) < low) {
        ndelta = get<1>(history[qid][++(get<0>(p))]);
        if (ndelta > 0) // enqueue
            get<1>(p) -= ndelta;
        else            // dequeue
            get<2>(p) += ndelta;
    }
    cout << "q" << qid << " moving sums: " << get<1>(p) << " " << get<2>(p) << endl;
}

void Zephyr::captureNow() {
    now = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
}

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