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
    : focals_(p),solver_(_s),pricing_manager_(_pm),accepter_(_acc),
      iterations_(DEFAULT_ITERS),time_limit_(DEFAULT_TLIMIT){
    solver_->compile();
    std::vector<id_val> original_alternative;
    for (int i = 0; i < solver_->nbEdges(); i++) {
        original_alternative.push_back(id_val(i,solver_->getConductance(i)));
    }
    addAlternative(original_alternative);
}

LocalSearchEngine::~LocalSearchEngine() {

}

void LocalSearchEngine::fillSolution(Solution& solution) {
    std::vector<id_val> dummy;
    solver_->getVoltages(solution.voltages_,dummy);
    solver_->getCurrents(solution.node_currents_, solution.edge_currents_);
    solution.objective_ = 0.0f;
    for (uint i = 0; i < focals_.size(); i++) {
        solution.objective_ += solution.voltages_[i][focals_[i].first].val;
    }
}
    
void LocalSearchEngine::findBaseSolution() {
    solver_->solve();

    fillSolution(base_solution_);
            
}


void LocalSearchEngine::findInitialSolution() {
    if (!base_solution_.isSet()) {
        throw std::runtime_error("findBaseSolution must be called before"
                                 " calling findInitialSolution!");
    }

    std::sort(base_solution_.edge_currents_.begin(),
              base_solution_.edge_currents_.end(),
              id_val::sort_by_val);
    pricing_manager_->reset();

    std::vector<id_val> updates;
    for (int i = solver_->nbEdges() - 1; i >= 0; i--) {
        int e = base_solution_.edge_currents_[i].id;
        if (pricing_manager_->consume(pricing_manager_->getCost(e))) {
            updates.push_back(id_val(e,getAlternativeConductance(1,e)));
            initial_solution_.choose(e,1);
        }
    }
    solver_->updateConductances(updates);
    std::cout<<"HERE "<<std::endl;
    initial_solution_.print(std::cout);
    std::cout<<"DONE"<<std::endl;
    solver_->solve();
    fillSolution(initial_solution_);
}
    
void LocalSearchEngine::solve() {
    if (!initial_solution_.isSet()) {
        throw std::runtime_error("findInitialSolution must be called before"
                                 " calling solve!");
    }
    int iter = -1;
    Solution current = initial_solution_;
    Solution accepted = initial_solution_;
    Solution best = initial_solution_;
    accepter_->reset(initial_solution_.objective_);

   
    std::clock_t begin_time = clock();

    while (iter < iterations_ &&
           float(clock() - begin_time)/CLOCKS_PER_SEC < time_limit_) {
        iter++;
        std::cout<<"Solve iteration "<< iter <<std::endl;
        std::vector<id_val> updates;
        //Destroy accepted solution
        Destroyer des(this,pricing_manager_,Destroyer::INVLC,Destroyer::WILHCP,5);
        des.destroy(accepted,current,updates);
        std::cout<<"Destroyed " <<std::endl;
        solver_->updateConductances(updates);
        solver_->solve();
        std::cout<<"Solved "<< std::endl;
        fillSolution(current);
        current.print(std::cout,1);
        if (current.objective_ < best.objective_) {
            best = current;
        }
        if (accepter_->accept(current.objective_)) {
            accepted = current;
        }
        
    }

    
}
