#ifndef DISTRIBUTION_MODEL_H
#define DISTRIBUTION_MODEL_H

#include <tuple>

#define dw1 0.80
#define dw2 0.10
#define dw3 0.10

using std::tuple;

class DistributionModel {
    public:
        virtual tuple<double, double, double> getWeights(tuple<int, int, int> sizes) = 0;
};

class Standard : public DistributionModel {
    public:
        tuple<double, double, double> getWeights(tuple<int, int, int> sizes) {
            return tuple<double, double, double>(dw1, dw2, dw3);
        }
};

class Zephyr : public DistributionModel {
    public:
        tuple<double, double, double> getWeights(tuple<int, int, int> sizes) {
            bool o1 = true;  // determine if queue is stressed
            bool o2 = true;
            bool o3 = true;
            if ((o1 and !o2 and !o3) or (!o1 and o2 and !o3) or (!o1 and !o2 and o3)) {
                if (o1) {
                    return tuple<double, double, double>(dw1, dw2, dw3);
                } else if (o2) {
                    return tuple<double, double, double>(dw1, dw2, dw3);
                } else {
                    return tuple<double, double, double>(dw1, dw2, dw3);
                }
            } else if ((o1 and !o2 and o3) or (o1 and o2 and !o3)) {
                return tuple<double, double, double>(dw1, dw2, dw3);

            } else if ((!o1 and o2 and o3)) {
                return tuple<double, double, double>(dw1, dw2, dw3);
            }
            return tuple<double, double, double>(dw1, dw2, dw3);
        }
};

#endif // DISTRIBUTION_MODEL_H
