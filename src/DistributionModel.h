#include <tuple>

using std::tuple;

class DistributionModel {
    public:
        virtual tuple<double, double, double> getWeights(tuple<int, int, int> sizes) = 0;
};

class Standard : public DistributionModel {
    public:
        tuple<double, double, double> getWeights(tuple<int, int, int> sizes) {
            return tuple<double, double, double>(0.80, 0.1, 0.1);
        }
};
