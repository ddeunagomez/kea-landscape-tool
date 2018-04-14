#ifndef SOLUTION_H
#define SOLUTION_H


#include <vector>
#include <unordered_map>
#include <algorithm>
#include <iomanip>
#include "pricing_manager.hpp"
#include "solver_iter_petsc.hpp"
#include "accepter.hpp"

class Solution {
private:
    std::unordered_map<int,int> chosen_alternative_;
public:
    void choose(int e, int alt) {
        chosen_alternative_[e] = alt;
    }
    bool chosen(int e) {
        if (chosen_alternative_.find(e) == chosen_alternative_.end())
            chosen_alternative_[e] = 0;
        return chosen_alternative_[e] != 0;
    }
    void discard(int e) {
        chosen_alternative_[e] = 0;
    }
    bool isSet() {
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

    void print(std::ostream& o, int level = 1) {
        o <<"R = "<<std::fixed<<std::setprecision(10)<<objective_<<"\n";
        if (level < 1) return;
        int i = 0;
        o <<"Sol = [";
        for (auto it = chosen_alternative_.begin(); it != chosen_alternative_.end(); ++it) {
            if (it->second != 0) {
                o<<it->first<<",";
                i++;
            }
        }
        o<<"]"<<i<<"\n";
        if (level < 2) return;
        o <<"Vn = [";
        for (uint j = 0; j < voltages_.size(); j++) {
            for (uint i = 0; i < voltages_[j].size(); i++) {
                o<<"("<<voltages_[j][i].id<<","<<voltages_[j][i].val<<"),";
            }
            if (j < voltages_.size() - 1)
                o<<"\n      ";
        }
        o<<"]\n";
        if (level < 3) return;
        o <<"Cn = [";
        for (uint i = 0; i < node_currents_.size(); i++) {
            o<<"("<<node_currents_[i].id<<","<<node_currents_[i].val<<"),";
        }
        o<<"]\n";
        if (level < 4) return;
        o <<"Ce = [";
        for (uint i = 0; i < edge_currents_.size(); i++) {
            o<<"("<<edge_currents_[i].id<<","<<edge_currents_[i].val<<"),";
        }
        o<<"]\n";
    }
};

#endif // SOLUTION_H
