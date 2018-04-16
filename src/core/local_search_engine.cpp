#include "local_search_engine.hpp"
#include "destroyer.hpp"
#include <algorithm>
#include <ctime>

using namespace std;

const double LocalSearchEngine::kDefaultAlternative = 1;

LocalSearchEngine::LocalSearchEngine(vector<pair<ElectricalCircuit::NodeID,ElectricalCircuit::NodeID> > p,
                                     Solver* _s, PricingManager* _pm,
                                     Accepter* _acc, const KeaOptions options)
    : focals_(p),solver_(_s),pricing_manager_(_pm),accepter_(_acc),
      options_(options){
    solver_->compile();
    vector<id_val> original_alternative;
    for (int i = 0; i < solver_->nbEdges(); i++) {
        original_alternative.push_back(id_val(i,solver_->getConductance(i)));
    }
    addAlternative(original_alternative);
}

LocalSearchEngine::~LocalSearchEngine() {

}

void LocalSearchEngine::fillSolution(Solution& solution) {
    vector<id_val> dummy;
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
        throw runtime_error("findBaseSolution must be called before"
                                 " calling findInitialSolution!");
    }

    sort(base_solution_.edge_currents_.begin(),
              base_solution_.edge_currents_.end(),
              id_val::sort_by_val);
    pricing_manager_->reset();

    vector<id_val> updates;
    for (int i = solver_->nbEdges() - 1; i >= 0; i--) {
        int e = base_solution_.edge_currents_[i].id;
        if (pricing_manager_->consume(pricing_manager_->getCost(e))) {
            updates.push_back(id_val(e,getAlternativeConductance(1,e)));
            initial_solution_.choose(e,1);
        }
    }
    solver_->updateConductances(updates);
    solver_->solve();
    fillSolution(initial_solution_);
}
    
Solution LocalSearchEngine::solve(JsonObject *solution_collector) {
    if (!initial_solution_.isSet()) {
        throw runtime_error("findInitialSolution must be called before"
                                 " calling solve!");
    }
    uint iterations = 0;
    uint solutions = 0;
    Solution current = initial_solution_;
    Solution accepted = initial_solution_;
    Solution best = initial_solution_;
    accepter_->reset(initial_solution_.objective_);

   
    clock_t begin_time = clock();

    while (iterations < options_.max_iterations &&
           float(clock() - begin_time)/CLOCKS_PER_SEC < options_.time_limit) {
        cout<<"\tLocal Search iteration "<< iterations <<endl;

        vector<id_val> updates;
        //Destroy accepted solution
        Destroyer des(this,pricing_manager_,Destroyer::kLeastCurrent,Destroyer::kHighCurrentProbability,options_.destruction_rate);
        cout<<"\tDestruction phase " <<endl;
        des.destroy(accepted,current,updates);

        solver_->updateConductances(updates);
        cout<<"\tSolving phase "<<endl;
        solver_->solve();
        cout<<"\tDone "<< endl;
        solutions++;
        fillSolution(current);
        if (solution_collector) {
            JsonObject* solution = current.toJson();
            //cout<<solution->toString()<<endl;
            //current.print(cout,10);
            solution_collector->add("solution_"+to_string(solutions - 1),solution);
        }
        if (current.objective_ < best.objective_) {
            best = current;
        }
        if (accepter_->accept(current.objective_)) {
            accepted = current;
        }
        iterations++;
        
    }
    if (solution_collector) {
        solution_collector->add("solutions_count",new JsonInt(solutions));
    }
    return best;

    
}
