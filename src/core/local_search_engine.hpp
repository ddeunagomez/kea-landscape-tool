#ifndef LOCAL_SEARCH_ENGINE
#define LOCAL_SEARCH_ENGINE


#include <vector>
#include <unordered_map>
#include <algorithm>
#include <iomanip>
#include "pricing_manager.hpp"
#include "solver_iter_petsc.hpp"
#include "accepter.hpp"
#include "solution.hpp"



class Destroyer;
class LocalSearchEngine {
    friend Destroyer;
private:
    Solution base_solution_;
    Solution initial_solution_;
    
    std::vector<std::pair<ElectricalCircuit::NodeID,ElectricalCircuit::NodeID> > focals_;

    Solver* solver_;
    PricingManager* pricing_manager_;
    Accepter* accepter_;
    //Alternative values
    std::vector< std::vector<id_val> > alternative_conductances_;
    static const double kDefaultAlternative;
    double getAlternativeConductance(uint alt_id, int e) const {
        if (alt_id < alternative_conductances_.size()) {
            if (alternative_conductances_[alt_id][e].id != e) {
                throw std::runtime_error("Unsupported incomplete alternative "
                                         "conductances, yet");
            }
            return alternative_conductances_[alt_id][e].val;
        }
        return kDefaultAlternative;
    }

    int iterations_;
    static const int kDefaultNumIterations;
    float time_limit_;
    static const float kDefaultTimeLimit;

    void fillSolution(Solution &solution);
    
public:
    LocalSearchEngine(std::vector<std::pair<ElectricalCircuit::NodeID, ElectricalCircuit::NodeID> > p,
                      Solver* _s, PricingManager* _pm, Accepter* _acc);

    virtual ~LocalSearchEngine();

    inline int nbFocals() const {
        return focals_.size();
    }
    
    inline void addAlternative(std::vector<id_val> alternative) {
        alternative_conductances_.push_back(alternative);
    }

    inline bool worthInvest(int e) {
        if (fabs(getAlternativeConductance(0,e) - getAlternativeConductance(1,e)) < 0.0001)
            return false;
        return true;
    }
    
    void findBaseSolution();

    inline Solution getBaseSolution() const {
        return base_solution_;
    }
    
    void findInitialSolution();

    inline Solution getInitialSolution() const {
        return initial_solution_;
    }

    inline void setMaxIterations(int i) {
        if (i <= 0)
            std::runtime_error("Cannot set number of iterations to < 1");
        iterations_ = i;
    }
    
    inline void setTimeLimit(float tl) {
        if (tl <= 0)
            std::runtime_error("Cannot set time limit to <= 0");
        time_limit_ = tl;
    }
    
    Solution solve(JsonObject* solution_collector = NULL);

    

};






#endif /* LOCAL_SEARCH_ENGINE */
