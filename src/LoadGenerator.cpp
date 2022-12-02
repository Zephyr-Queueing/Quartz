#include <iostream>
#include <chrono>
#include <thread>
#include <random>

#include <Message.h>
#include <LoadGenerator.h>
#include <WeightedPriorityQueue.h>

using namespace std;

static int randomPriority(uniform_real_distribution<> &unif, mt19937 &gen, const tuple<double, double, double> &dist);
static Message create(int priority, string data);

void LG::operator()() {
    random_device rd;
    mt19937 gen(rd());
    exponential_distribution<> exp(rate);
    uniform_real_distribution<> unif(0, 1);

    while (true) {
        this_thread::sleep_for(chrono::milliseconds(static_cast<int>(1000 * exp(gen))));
        int p = randomPriority(unif, gen, dist);
        Message msg(create(p, "mydata"));
        wpq.enqueue(msg);
    }
}

static int randomPriority(uniform_real_distribution<> &unif, mt19937 &gen, const tuple<double, double, double> &dist) {
    double n = unif(gen);
    if (n <= get<0>(dist))
        return 1;
    if (n <= get<0>(dist) + get<1>(dist))
        return 2;
    return 3;
}

static Message create(int priority, string data) {
    chrono::milliseconds enqueueTime = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch());
    chrono::milliseconds dequeueTime = milliseconds(0);
    Message msg(priority, data, enqueueTime, dequeueTime);
    return msg;
}
