#include <tuple>

#include <DistributionModel.h>

using std::tuple;

tuple<double, double, double> Standard::getWeights(tuple<int, int, int> sizes) {
    return tuple<double, double, double>(dw1, dw2, dw3);
}

tuple<double, double, double> Zephyr::getWeights(tuple<int, int, int> sizes) {
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