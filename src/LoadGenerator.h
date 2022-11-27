#ifndef LOAD_GENERATOR_H
#define LOAD_GENERATOR_H

#include <iostream>

#include <WeightedPriorityQueue.h>

using namespace std;

class LG {
  public:
    void operator()(double rate, tuple<double, double, double> dist, WeightedPriorityQueue &wpq);
};

#endif // LOAD_GENERATOR_H
