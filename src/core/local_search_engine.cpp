#include "local_search_engine.hpp"
#include "destroyer.hpp"
#include <algorithm>
#include <ctime>

const double LocalSearchEngine::DEFAULT_ALT = 1;
const int LocalSearchEngine::DEFAULT_ITERS = 10;
const float LocalSearchEngine::DEFAULT_TLIMIT = 200.0;

LocalSearchEngine::LocalSearchEngine(std::vector<std::pair<int,int> > p,
                                     Solver* _s, PricingManager* _pm,
                                     Accepter* _acc)
    : focals(p),s(_s),pm(_pm),acc(_acc),
      iterations(DEFAULT_ITERS),time_limit(DEFAULT_TLIMIT){
    s->compile();
    std::vector<id_val> original_alternative;
    for (int i = 0; i < s->nbEdges(); i++) {
        original_alternative.push_back(id_val(i,s->getCond(i)));
    }
    addAlt(original_alternative);
}

LocalSearchEngine::~LocalSearchEngine() {

}

void LocalSearchEngine::fillSolution(Solution& solution) {
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

    std::vector<id_val> updates;
    for (int i = s->nbEdges() - 1; i >=0; i--) {
        int e = base_sol.currents_e[i].id;
        if (pm->consume(pm->getCost(e))) {
            updates.push_back(id_val(e,getAlt(1,e)));
            init_sol.choose(e,1);
        }
    }
    s->updateConductances(updates);

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
    Solution current = init_sol;
    Solution accepted = init_sol;
    Solution best = init_sol;
    acc->reset(init_sol.obj);

   
    std::clock_t begin_time = clock();

    while (iter < iterations &&
           float(clock() - begin_time)/CLOCKS_PER_SEC < time_limit) {        
        iter++;
        std::cout<<"Solve iteration "<< iter <<std::endl;
        std::vector<id_val> updates;
        //Destroy accepted solution
        Destroyer des(this,pm,Destroyer::INVRAND,Destroyer::WILRAND,5);
        des.destroy(accepted,current,updates);
        std::cout<<"Destroyed " <<std::endl;
        s->updateConductances(updates);
        s->solve();
        std::cout<<"Solved "<< std::endl;
        fillSolution(current);
        current.print(std::cout,1);
        if (current.obj < best.obj) {
            best = current;
        }
        if (acc->accept(current.obj)) {
            accepted = current;
        }
        
    }

    
}
