#include <tuple>

using std::tuple;

class DistributionModel {
    public:
        virtual tuple<int, int, int> getWeights(tuple<int, int, int> sizes);
};
