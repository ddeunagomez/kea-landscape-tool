
#include "local_search_engine.hpp"

LocalSearchEngine::LocalSearchEngine(std::vector<std::pair<int,int> > p,
                                     Solver* _s, PricingManager* _pm)
    : focals(p),s(_s),pm(_pm){

    s->compile();

}

LocalSearchEngine::~LocalSearchEngine() {

}

    
double LocalSearchEngine::baseSolution() {
    s->solve();

    std::vector<std::vector<id_val> > vss;
    std::vector<id_val> dummy;
    s->getVoltages(vss,dummy);

    double total = 0.0;
    for (uint i = 0; i < focals.size(); i++) {
        total += vss[i][focals[i].first].val;
    }
    
    
    return total;
}

double LocalSearchEngine::initialSolution() {
    return 0.0;
}
    
void LocalSearchEngine::solve() {

}
