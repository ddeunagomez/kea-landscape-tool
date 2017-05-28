#ifndef LOCAL_SEARCH_ENGINE
#define LOCAL_SEARCH_ENGINE


#include <vector>
#include "pricing_manager.hpp"
#include "solver_iter_petsc.hpp"

class LocalSearchEngine {

    std::vector<std::pair<int,int> > focals;

    Solver* s;
    PricingManager* pm;
    //base values
    std::vector<id_val> base;
    //Alternative values
    std::vector< std::vector<id_val> > alts;
    const double DEFAULT_ALT = 10;

    const int DEFAULT_ITERS = 10;
public:
    LocalSearchEngine(std::vector<std::pair<int,int> > p,
                      Solver* _s, PricingManager* _pm);

    virtual ~LocalSearchEngine();

    
    inline void addAlt(std::vector<id_val> alternative) {
        alts.push_back(alternative);
    }

    double baseSolution();

    double initialSolution();
    
    void solve();

    

};






#endif /* LOCAL_SEARCH_ENGINE */
