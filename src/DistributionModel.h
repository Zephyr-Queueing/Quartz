#ifndef DISTRIBUTION_MODEL_H
#define DISTRIBUTION_MODEL_H

#include <tuple>
#include <chrono>
#include <thread>
#include <map>
#include <tuple>
#include <vector>

#define DW1 0.8
#define DW2 0.1
#define DW3 0.1

using std::tuple;

using namespace std;
using namespace chrono;

class DistributionModel {
  public:
    DistributionModel() : dw1(DW1), dw2(DW2), dw3(DW3) {};
    virtual tuple<double, double, double> getWeights(const tuple<int, int, int> &sizes) = 0;
    virtual void notify(int qid, int delta) = 0;
    virtual ~DistributionModel() {};

  protected:
    double dw1;
    double dw2;
    double dw3;
};

class Standard : public DistributionModel {
  public:
    Standard() = default;
    virtual tuple<double, double, double> getWeights(const tuple<int, int, int> &sizes);
    virtual void notify(int qid, int delta);
    virtual ~Standard() {};
};

class Zephyr : public DistributionModel {
  public:
    Zephyr();
    virtual void notify(int qid, int delta);
    virtual tuple<double, double, double> getWeights(const tuple<int, int, int> &sizes);
    virtual ~Zephyr() {};
  
  private:
    void captureNow();
    void refreshAllMovingSums();
    void refreshQueueMovingSums(int qid);
    double estimateSpareWeight(int qid, const tuple<int, int, int> &sizes);
    map<int, tuple<int, int, int>> movingSums; // qid -> least-recent history idx, moving enqueue sum, moving dequeue sum
    map<int, vector<tuple<milliseconds, int>>> history;
    milliseconds now;
};

#endif // DISTRIBUTION_MODEL_H
