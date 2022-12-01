#include <iostream>
#include <tuple>
#include <vector>
#include <map>
#include <chrono>
#include <thread>
#include <assert.h>
#include <DistributionModel.h>

// #define DEBUG
#define MSG_THRESHOLD 2
#define MAX_WINDOW 5000 // ms
#define MAX_Q_DELAY 500 // ms

#ifdef DEBUG 
#define DEBUG_BUILD(x) x
#else 
#define DEBUG_BUILD(x)
#endif

using std::tuple;
using chrono::milliseconds;

using namespace std;

static int getQueueSize(int qid, tuple<int, int, int> &sizes);
static double getQueueWeight(int qid);
static void normalizeWeights(tuple<double, double, double> &dws);

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
    tuple<double, double, double> dws(dw1, dw2, dw3);
    if (history[0].size() < MSG_THRESHOLD ||
        history[1].size() < MSG_THRESHOLD ||
        history[2].size() < MSG_THRESHOLD) {
        return dws;
    }
    refreshAllMovingSums();
    double o1Spare = estimateSpareWeight(1, sizes);
    double o2Spare = estimateSpareWeight(2, sizes);
    double o3Spare = estimateSpareWeight(3, sizes);
    double totSpare = o1Spare + o2Spare + o3Spare;
    // cout << o1Spare << ", " << o2Spare << ", " << o3Spare << endl;
    int numStressed = int(o1Spare > 0.0) + int(o2Spare > 0.0) + int(o3Spare > 0.0);
    if (numStressed != 0) {
        get<0>(dws) += totSpare / 3.0 - o1Spare;
        get<1>(dws) += totSpare / 3.0 - o2Spare;
        get<2>(dws) += totSpare / 3.0 - o3Spare;
    }
    normalizeWeights(dws);
    cout << "Weights: " << get<0>(dws) << ", " << get<1>(dws) << ", " << get<2>(dws) << endl; 
    DEBUG_BUILD(
        cout << "Queue 1 > Size: " << get<0>(sizes) << "\tSpare Weight: " << o1Spare << endl;
        cout << "Queue 2 > Size: " << get<1>(sizes) << "\tSpare Weight: " << o2Spare << endl;
        cout << "Queue 3 > Size: " << get<2>(sizes) << "\tSpare Weight: " << o3Spare << endl;
    )
    return dws;
}

void Zephyr::notify(int qid, int delta) {
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
    tuple<int, int, int> &ms = movingSums[qid - 1];
    vector<tuple<milliseconds, int>> &qhist = history[qid];
    int64_t win = MAX_WINDOW;
    assert(win <= MAX_WINDOW);
    double enqueueRate = (double) get<1>(ms) / win;
    double dequeueRate = (double) get<2>(ms) / win;
    double netThroughput = dequeueRate - enqueueRate;

    // Compute drain delay.
    double drainDelay;
    if (netThroughput > 0)
        drainDelay = (double) getQueueSize(qid, sizes) / netThroughput;
    else
        drainDelay = std::numeric_limits<double>::infinity();

    DEBUG_BUILD(
        cout << "time (unix)"<< ",\t\t"
            << "queue id" << ",\t\t"
            << "queue size" << ",\t\t" 
            << "enqeue sum" << ",\t\t"
            << "deqeue sum" << ",\t\t"
            << "net throughput" << ",\t\t"
            << "drain delay" << endl;
        cout << now.count() << ",\t\t"
            << qid << ",\t\t"
            << getQueueSize(qid, sizes) << ",\t\t" 
            << (double) get<1>(ms) << ",\t\t"
            << (double) get<2>(ms) << ",\t\t"
            << netThroughput << ",\t\t"
            << drainDelay << endl;
    )

    // Return no spare weight if stressed.
    if (netThroughput <= 0 || drainDelay > MAX_Q_DELAY) {
        return 0.0;
    }

    // Return well-behaved spare weight.
    enqueueRate = max(0.001, enqueueRate);
    return netThroughput / enqueueRate;
}

void Zephyr::refreshAllMovingSums() {
    for (int i = 0; i <= 2; i++) {
        refreshQueueMovingSums(i);
    }
}

void Zephyr::refreshQueueMovingSums(int qid) {
    int64_t low = now.count() - MAX_WINDOW;
    tuple<int, int, int> &p = movingSums[qid];
    int ndelta;
    while (get<0>(p) < history[qid].size() - 1 && get<0>(history[qid][get<0>(p)]).count() < low) {
        ndelta = get<1>(history[qid][++(get<0>(p))]);
        if (ndelta > 0) // enqueue
            get<1>(p) -= ndelta;
        else            // dequeue
            get<2>(p) += ndelta;
    }
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

static void normalizeWeights(tuple<double, double, double> &dws) {
    double m = get<0>(dws);
    m = min(m, get<1>(dws));
    m = min(m, get<2>(dws));

    if (m < 0) {
        get<0>(dws) -= m;
        get<1>(dws) -= m;
        get<2>(dws) -= m;
    }

    double tot = get<0>(dws) + get<1>(dws) + get<2>(dws);
    // cout << tot << endl;
    get<0>(dws) /= tot;
    get<1>(dws) /= tot;
    get<2>(dws) /= tot;
}
