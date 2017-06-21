#ifndef LOCAL_SEARCH_ENGINE
#define LOCAL_SEARCH_ENGINE


#include <vector>
#include <unordered_map>
#include <algorithm>
#include "pricing_manager.hpp"
#include "solver_iter_petsc.hpp"
#include "accepter.hpp"

class Solution {
private:
    std::unordered_map<int,int> chosen_alt;    
public:
    void choose(int e, int alt) {
        chosen_alt[e] = alt;
    }
    bool chosen(int e) {
        if (chosen_alt.find(e) == chosen_alt.end())
            chosen_alt[e] = 0;
        return chosen_alt[e] != 0;
    }
    void discard(int e) {
        chosen_alt[e] = 0;
    }
    Solution() { }
    double obj;
    std::vector<std::vector<id_val> > voltages;
    std::vector<id_val> currents_n;
    std::vector<id_val> currents_e;
    bool operator==(const Solution& s2) const {        
        return chosen_alt == s2.chosen_alt;
    }
    void print(std::ostream& o, int level = 1) {            
        o <<"R = "<<obj<<"\n";
        if (level < 1) return;
        int i = 0;
        o <<"Sol = [";
        for (auto it = chosen_alt.begin(); it != chosen_alt.end(); ++it) {
            if (it->second != 0) {
                o<<it->first<<",";
                i++;
            }
        }
        o<<"]"<<i<<"\n";
        if (level < 2) return;
        o <<"Vn = [";
        for (uint j = 0; j < voltages.size(); j++) {
            for (uint i = 0; i < voltages[j].size(); i++) {
                o<<"("<<voltages[j][i].id<<","<<voltages[j][i].val<<"),";
            }
            if (j < voltages.size() - 1)
                o<<"\n      ";
        }
        o<<"]\n";
        if (level < 3) return;
        o <<"Cn = [";
        for (uint i = 0; i < currents_n.size(); i++) {
            o<<"("<<currents_n[i].id<<","<<currents_n[i].val<<"),";
        }
        o<<"]\n";
        if (level < 4) return;
        o <<"Ce = [";
        for (uint i = 0; i < currents_e.size(); i++) {
            o<<"("<<currents_e[i].id<<","<<currents_e[i].val<<"),";
        }
        o<<"]\n";
    }
};

class Destroyer;
class LocalSearchEngine {
    friend Destroyer;
private:
    Solution base_sol;
    Solution init_sol;
    
    std::vector<std::pair<int,int> > focals;

    Solver* s;
    PricingManager* pm;
    Accepter* acc;
    //base values
    std::vector<id_val> base;
    //Alternative values
    std::vector< std::vector<id_val> > alts;
    static const double DEFAULT_ALT;
    double getAlt(uint alt_id, int e) const {
        if (alt_id < alts.size()) {
            if (alts[alt_id][e].id != e) {
                throw std::runtime_error("Unsupported incomplete alternative "
                                         "conductances, yet");
            }
            return alts[alt_id][e].val;
        }
        return DEFAULT_ALT;            
    }

    int iterations;
    static const int DEFAULT_ITERS;
    float time_limit;
    static const float DEFAULT_TLIMIT;

    void fillSolution(Solution& solution);
    
public:
    LocalSearchEngine(std::vector<std::pair<int,int> > p,
                      Solver* _s, PricingManager* _pm, Accepter* _acc);

    virtual ~LocalSearchEngine();

    inline int nbFocals() const {
        return focals.size();
    }
    
    inline void addAlt(std::vector<id_val> alternative) {
        alts.push_back(alternative);
    }

    inline bool worthInvest(int e) {
        if (fabs(getAlt(0,e) - getAlt(1,e)) < 0.0001)
            return false;
        return true;
    }
    
    void findBaseSolution();

    inline Solution getBaseSolution() const {
        return base_sol;
    }
    
    void findInitialSolution();

    inline Solution getInitialSolution() const {
        return init_sol;
    }

    inline void setMaxIterations(int i) {
        if (i <= 0)
            std::runtime_error("Cannot set number of iterations to < 1");
        iterations = i;
    }
    
    inline void setTimeLimit(float tl) {
        if (tl <= 0)
            std::runtime_error("Cannot set time limit to <= 0");
        time_limit = tl;
    }
    
    void solve();

    

};






#endif /* LOCAL_SEARCH_ENGINE */
