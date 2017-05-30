#include "local_search_engine.hpp"

#include <algorithm>


LocalSearchEngine::LocalSearchEngine(std::vector<std::pair<int,int> > p,
                                     Solver* _s, PricingManager* _pm)
    : focals(p),s(_s),pm(_pm){

    s->compile();

}

LocalSearchEngine::~LocalSearchEngine() {

}

void LocalSearchEngine::fillSolution(struct solution& solution) {
    std::vector<id_val> dummy;
    s->getVoltages(solution.voltages,dummy);
    s->getCurrents(solution.currents_n, solution.currents_e);
    solution.obj = 0.0;
    for (uint i = 0; i < focals.size(); i++) {
        solution.obj += solution.voltages[i][focals[i].first].val;
    }
}
    
void LocalSearchEngine::findBaseSolution() {
    s->solve();

    fillSolution(base_sol);
            
}


void LocalSearchEngine::findInitialSolution() {
    if ((int)base_sol.currents_n.size() != s->nbNodes() ||
        (int)base_sol.currents_e.size() != s->nbEdges()) {
        throw std::runtime_error("findBaseSolution must be called before"
                                 " calling findInitialSolution!");
    }

    std::sort(base_sol.currents_e.begin(),
              base_sol.currents_e.end(),
              id_val::sort_by_val);
    pm->reset();

    base_sol.print(std::cout,4);
    
    std::vector<ECircuit::EdgeID> edges;
    std::vector<double> vals;
    for (int i = s->nbEdges() - 1; i >=0; i--) {
        int e = base_sol.currents_e[i].id;
        if (pm->consume(pm->getCost(e))) {
            edges.push_back(e);
            vals.push_back(getAlt(1,e));
            init_sol.chosen_alt.push_back(id_val(e,1));
        }
    }
    s->updateConductances(edges,vals);

    s->solve();
    fillSolution(init_sol);
}
    
void LocalSearchEngine::solve() {
    if ((int)init_sol.currents_n.size() != s->nbNodes() ||
        (int)init_sol.currents_e.size() != s->nbEdges()) {
        throw std::runtime_error("findInitialSolution must be called before"
                                 " calling solve!");
    }
    int iter = -1;
    struct solution current = init_sol;
    struct solution accepted = init_sol;
    struct solution best = init_sol;

    while (iter < DEFAULT_ITERS) {
        iter++;
        std::vector<ECircuit::EdgeID> edges;
        std::vector<double> vals;
        //Destroy accepted solution
        //Create new soluition -> edge & vals
        s->updateConductances(edges,vals);
        s->solve();
        fillSolution(current);
        if (current.obj < best.obj) {
            best = current;
        }
        //Accept? (current vs accepted) -> new value for accepted
        
    }

    
}
