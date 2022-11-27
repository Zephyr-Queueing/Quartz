#include <tuple>

using std::tuple;

class DistributionModel {
    public:
        virtual tuple<int, int, int> getWeights(int batchSize, tuple<int, int, int> sizes) = 0;
};

class Standard : public DistributionModel {
    public:
        tuple<int, int, int> getWeights(int batchSize, tuple<int, int, int> sizes) {
            return tuple(0.80 * batchSize, 0.1 * batchSize, 0.1 * batchSize);
        }
};
