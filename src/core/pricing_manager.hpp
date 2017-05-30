#ifndef PRINCING_MANAGER_H
#define PRINCING_MANAGER_H

#include <vector>
#include "utils.hpp"

class PricingManager {

    const double DEFAULT_COST = 1;
    double budget;
    double current_budget;
    std::vector<id_val> costs;

public:
    PricingManager(double b, std::vector<id_val> cs = std::vector<id_val>())
        : budget(b), current_budget(b), costs(cs){

    };

    inline double getCost(int e) {
        if (costs.size() == 0) {
            return DEFAULT_COST;
        }
        return costs[e].val;
    }
    inline bool consume(double v) {
        if (current_budget - v >= 0) {
            current_budget -= v;
            return true;
        }
        return false;
    }
    inline void reset() {
        current_budget = budget;
    }
    inline double budgetLeft() const {
        return current_budget;
    }
    inline double initialBudget() const {
        return budget;
    }
};




#endif /*  PRINCING_MANAGER_H */
