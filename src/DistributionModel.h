#include <tuple>

using std::tuple;

class DistributionModel {
    public:
        virtual tuple<int, int, int> getWeights(int batchSize, tuple<int, int, int> sizes) = 0;
};
