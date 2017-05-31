#ifndef LOCAL_SEARCH_ENGINE
#define LOCAL_SEARCH_ENGINE


#include <vector>
#include "pricing_manager.hpp"
#include "solver_iter_petsc.hpp"
#include "accepter.hpp"

class LocalSearchEngine {
public:
    struct solution {
        double obj;
        std::vector<std::vector<id_val> > voltages;
        std::vector<id_val> currents_n;
        std::vector<id_val> currents_e;
        std::vector<id_val> chosen_alt;
        void print(std::ostream& o, int level = 1) {            
            o <<"R = "<<obj<<"\n";
            if (level < 1) return;
            o <<"Sol = [";
            for (uint i = 0; i < chosen_alt.size(); i++) {
                o<<chosen_alt[i].id<<",";
            }
            o<<"]\n";
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
private:
    struct solution base_sol;
    struct solution init_sol;
    
    std::vector<std::pair<int,int> > focals;

    Solver* s;
    PricingManager* pm;
    Accepter* acc;
    //base values
    std::vector<id_val> base;
    //Alternative values
    std::vector< std::vector<id_val> > alts;
    const double DEFAULT_ALT = 1;
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

    const int DEFAULT_ITERS = 10;

    void fillSolution(struct solution& solution);
    
public:
    LocalSearchEngine(std::vector<std::pair<int,int> > p,
                      Solver* _s, PricingManager* _pm, Accepter* _acc);

    virtual ~LocalSearchEngine();

    
    inline void addAlt(std::vector<id_val> alternative) {
        alts.push_back(alternative);
    }

    void findBaseSolution();
    inline struct solution getBaseSolution() const {
        return base_sol;
    }
    void findInitialSolution();
    inline struct solution getInitialSolution() const {
        return init_sol;
    }
    
    void solve();

    

};






#endif /* LOCAL_SEARCH_ENGINE */
