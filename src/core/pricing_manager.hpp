#ifndef PRINCING_MANAGER_H
#define PRINCING_MANAGER_H

#include <vector>
#include "utils.hpp"

class PricingManager {

    const double DEFAULT_COST = 1;
    double budget_;
    double current_budget_;
    std::vector<id_val> costs_;

public:
    PricingManager(double b, std::vector<id_val> cs = std::vector<id_val>())
        : budget_(b), current_budget_(b), costs_(cs){

    };

    inline double getCost(int e) {
        if (costs_.size() == 0) {
            return DEFAULT_COST;
        }
        return costs_[e].val;
    }
    inline bool consume(double v) {
        if (current_budget_ - v >= 0) {
            current_budget_ -= v;
            return true;
        }
        return false;
    }
    inline void save(double how_much) {
        current_budget_ += how_much;
        if (current_budget_ > budget_) //For precission error
            current_budget_ = budget_;
    }
    inline void reset() {
        current_budget_ = budget_;
    }
    inline double budgetLeft() const {
        return current_budget_;
    }
    inline double initialBudget() const {
        return budget_;
    }
};




#endif /*  PRINCING_MANAGER_H */
