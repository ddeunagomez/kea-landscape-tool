
#include "local_search_engine.hpp"

LocalSearchEngine::LocalSearchEngine(std::vector<std::pair<int,int> > p,
                                     Solver* _s, PricingManager* _pm)
    : focals(p),s(_s),pm(_pm){
    

}

LocalSearchEngine::~LocalSearchEngine() {

}

    
double LocalSearchEngine::baseSolution() {
    return 0.0;
}

double LocalSearchEngine::initialSolution() {
    return 0.0;
}
    
void LocalSearchEngine::solve() {

}
