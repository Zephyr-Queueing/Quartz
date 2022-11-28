#ifndef LOAD_GENERATOR_H
#define LOAD_GENERATOR_H

#include <iostream>

#include <WeightedPriorityQueue.h>

using namespace std;

class LG {
  public:
    LG(double r, tuple<double, double, double> d, WeightedPriorityQueue &w) : rate(r), dist(d), wpq(w) {}
    void operator()();
  
  private:
    double rate;
    tuple<double, double, double> dist;
    WeightedPriorityQueue &wpq;
};

#endif // LOAD_GENERATOR_H
