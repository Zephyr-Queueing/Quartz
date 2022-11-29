#ifndef DISTRIBUTION_MODEL_H
#define DISTRIBUTION_MODEL_H

#include <tuple>

#define DW1 0.8
#define DW2 0.1
#define DW3 0.1

using std::tuple;

class DistributionModel {
  public:
    DistributionModel() : dw1(DW1), dw2(DW2), dw3(DW3) {};
    virtual tuple<double, double, double> getWeights(tuple<int, int, int> sizes) = 0;
    virtual ~DistributionModel() {};

  protected:
    double dw1;
    double dw2;
    double dw3;
};

class Standard : public DistributionModel {
  public:
    Standard() = default;
    virtual tuple<double, double, double> getWeights(tuple<int, int, int> sizes);
    virtual ~Standard() {};
};

class Zephyr : public DistributionModel {
  public:
    Zephyr() = default;
    virtual tuple<double, double, double> getWeights(tuple<int, int, int> sizes);
    virtual ~Zephyr() {};
};

#endif // DISTRIBUTION_MODEL_H
