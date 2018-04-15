#ifndef SOLUTION_H
#define SOLUTION_H


#include <vector>
#include <unordered_map>
#include <algorithm>
#include <iomanip>
#include "pricing_manager.hpp"
#include "solver_iter_petsc.hpp"
#include "accepter.hpp"
#include "json.hpp"

class Solution {
private:
    std::unordered_map<int,int> chosen_alternative_;
public:
    inline void choose(int e, int alt) {
        chosen_alternative_[e] = alt;
    }
    inline bool chosen(int e) {
        if (chosen_alternative_.find(e) == chosen_alternative_.end())
            chosen_alternative_[e] = 0;
        return chosen_alternative_[e] != 0;
    }
    inline void discard(int e) {
        chosen_alternative_[e] = 0;
    }
    inline bool isSet() {
        return node_currents_.size() && edge_currents_.size() && voltages_.size();
    }

    float objective_;
    std::vector<std::vector<id_val> > voltages_;
    std::vector<id_val> node_currents_;
    std::vector<id_val> edge_currents_;

    Solution() : objective_(0) {

    }

    bool operator==(const Solution& s2) const {
        return chosen_alternative_ == s2.chosen_alternative_;
    }

    void print(std::ostream& o, int level = 1) const;
    JsonObject* toJson() const;

};

#endif // SOLUTION_H
