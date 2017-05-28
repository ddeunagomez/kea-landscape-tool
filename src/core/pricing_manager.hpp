#ifndef PRINCING_MANAGER_H
#define PRINCING_MANAGER_H

#include <vector>
#include "utils.hpp"

class PricingManager {

    const double DEFAULT_COST = 1;
    double budget;
    std::vector<id_val> costs;

public:
    PricingManager(double b, std::vector<id_val> cs = std::vector<id_val>())
        : budget(b), costs(cs){

    };

    double getCost(int e) {
        if (costs.size() < e) {
            return DEFAULT_COST;
        }
        return costs[e].val;
    }
    
};




#endif /*  PRINCING_MANAGER_H */
