#include "destroyer.hpp"
#include <algorithm>

void Destroyer::destroy(Solution& sol,
                        Solution& new_sol,
                        std::vector<id_val>& model_update) {

    consumed_rate_ = 0;
    changed_ = std::vector<bool>(sol.edge_currents_.size(),false);

    
    switch(remove_strategy_) {
    case kRandomRemove:
        destroyRandomEdges(sol,new_sol,model_update);
        break;
    case kLeastCost:
        destroyLeastCurrentEdges(sol,new_sol,model_update);
        break;
    case kLowCostProbability:
        destroyLowCurrentEdgesProb(sol,new_sol,model_update);
        break;
    default:
        throw std::runtime_error("Unknown LP destroyer for RemoveInv");
    }
    
    switch(add_strategy_) {
    case kRandomAdd:
        addRandomEdges(sol,new_sol,model_update);
        break;
    case kBfs:
        addBfsEdges(sol,new_sol,model_update);
        break;
    case kBfsRandom:
        addBfsRandomEdges(sol,new_sol,model_update);
        break;
    case kHighestCurrent:
        addHighestCurrentEdges(sol,new_sol,model_update);
        break;
    case kHigCurrentProbability:
        addHighCurrentEdgesProb(sol,new_sol,model_update);
        break;
    default:
        throw std::runtime_error("Unknown LP destroyer for RemoveInv");
    }


    if (sol == new_sol) {
        std::cout<<"WARNING: Stuck in local minimum. Getting out now."
                 <<std::endl;;
        // Call a probabilistic method to get out of local minimum
        destroyLeastCurrentEdges(sol,new_sol,model_update);
        addHighCurrentEdgesProb(sol,new_sol,model_update);
    }
    
}


void Destroyer::destroyLeastCurrentEdges(Solution& sol,
                                Solution& new_sol,
                                std::vector<id_val>& model_update) {

    std::sort(sol.edge_currents_.begin(), sol.edge_currents_.end(), id_val::sort_by_val);

    uint i = 0;
    while (i < sol.edge_currents_.size() && hasAvailableRate()) {
        int edge = sol.edge_currents_[i].id;
        if (sol.chosen(edge)) {
            removeInvestment(edge);      //Account edge e into rate
            changed_[edge] = true; //Do not invest on e, I just disinvested!
            model_update.push_back(id_val(edge,local_search_->getAlternativeConductance(0,edge))); //Set to original
            //std::cout<<"Alternative "<<model_update.back().id
            //<<" "<<model_update.back().val<<std::endl;
            new_sol.discard(edge);
            consumed_rate_++;
        }
        i++;
    }

}


void Destroyer::destroyRandomEdges(Solution& sol,
                        Solution& new_sol,
                        std::vector<id_val>& model_update) {
    std::vector<int> deck(sol.edge_currents_.size());
    std::iota(deck.begin(), deck.end(),0);
    std::random_shuffle(deck.begin(),deck.end());
    int i = 0;
    while (i < sol.edge_currents_.size() && hasAvailableRate()) {
        int edge = deck[i];
        if (sol.chosen(edge)) {
            removeInvestment(edge);
            changed_[edge] = true;
            model_update.push_back(id_val(edge,local_search_->getAlternativeConductance(0,edge))); //Set to original
            new_sol.discard(edge);
            consumed_rate_++;
        }

        i++;
    }
}


void Destroyer::destroyLowCurrentEdgesProb(Solution& sol,
                        Solution& new_sol,
                        std::vector<id_val>& model_update) {
    throw std::runtime_error("Not implemented yet");
}

float Destroyer::getMaxCurrent(Solution& sol) {
    float max_curr = 0.0;
    int nb_edges = sol.edge_currents_.size();
    for (int i = 0; i < nb_edges; i++) {
        max_curr = MAX(max_curr,sol.edge_currents_[i].val);
    }
    return max_curr;
}

void Destroyer::addHighCurrentEdgesProb(Solution& sol,
                                 Solution& new_sol,
                                 std::vector<id_val>& model_update) {

    float max_curr = getMaxCurrent(sol);
    float threshold = max_curr/(10.0*local_search_->nbFocals()); //TODO: option for this

    std::vector<id_val> probabilities;
    int nb_edges = sol.edge_currents_.size();

    float gambling_budget = 0;
    for (int edge = 0; edge < nb_edges; edge++) {
        if(sol.edge_currents_[edge].val < threshold)
            continue;
        if (!local_search_->worthInvest(edge))
            continue;
        if (sol.chosen(edge) || changed_[edge])
            continue;

        float edge_current = sol.edge_currents_[edge].val;
        probabilities.push_back(id_val(edge, edge_current +
                                       (probabilities.size() ? probabilities.back().val : 0.0)));
        gambling_budget += pricing_manager_->getCost(edge);
    }

    //I can buy more than I selected
    if (gambling_budget < pricing_manager_->budgetLeft()){
        std::vector<int> deck(nb_edges);
        std::iota(deck.begin(), deck.end(),0);
        std::random_shuffle(deck.begin(),deck.end());
        for (int i = 0;
             i < nb_edges && gambling_budget <= pricing_manager_->budgetLeft();
             i++) {
            int edge = deck[i];
            if (!local_search_->worthInvest(edge))
                continue;
            if (sol.chosen(edge) || changed_[edge])
                continue;
            float edge_current = sol.edge_currents_[edge].val;
            if(edge_current < threshold) {
                probabilities.push_back(id_val(edge, edge_current +
                                               (probabilities.size() ? probabilities.back().val : 0.0)));
                gambling_budget += pricing_manager_->getCost(edge);
            }
        }
    } 

    assert(gambling_budget >= pricing_manager_->budgetLeft());

    //In case I hit exactly the budget:
    if (gambling_budget == pricing_manager_->budgetLeft()) {
        for (uint i = 0; i < probabilities.size(); i++) {
            int e = probabilities[i].id;
            model_update.push_back(id_val(e,local_search_->getAlternativeConductance(1,e))); 
            new_sol.choose(e,local_search_->getAlternativeConductance(1,e));
        }
        return;
    }

    //General case:
    while(pricing_manager_->budgetLeft() > 0 && probabilities.size() > 0) {

        double val = probabilities.back().val
            * (double)rand() / (double)(RAND_MAX);
        int lo = 0; int hi = probabilities.size() - 1;
        int mid = lo + (hi - lo)/2;
        //Binary search for the edge with that value
        while (lo < hi) {
            mid = lo + (hi - lo)/2;
            if (mid == 0 ||
                (probabilities[mid - 1].val < val
                 && probabilities[mid].val >= val)) {
                break;
            } else if (probabilities[mid].val > val) {
                hi = mid;
            } else if (probabilities[mid].val < val) {
                lo = mid + 1;
            } else {
                throw std::runtime_error("Problem while finding uninvested edge"
                                         " to free (probably probabilites/"
                                         "currents are wrong)");
            }
        }
        int edge = probabilities[mid].id;
        float ecurr = sol.edge_currents_[edge].val;
        for (uint i = mid; i < probabilities.size() - 1; i++) {
            probabilities[i].val -= ecurr;
            probabilities[i].id = probabilities[i+1].id;
        }
        probabilities.pop_back();
        if (pricing_manager_->consume(pricing_manager_->getCost(edge))) {
            model_update.push_back(id_val(edge,local_search_->getAlternativeConductance(1,edge)));
            new_sol.choose(edge,local_search_->getAlternativeConductance(1,edge));
    	}
    }
    
}

void Destroyer::addRandomEdges(Solution &sol, Solution &new_sol, std::vector<id_val> &model_update) {

    std::vector<int> deck(sol.edge_currents_.size());
    std::iota(deck.begin(), deck.end(),0);
    std::random_shuffle(deck.begin(),deck.end());
    int i = 0;
    while(i < deck.size() && pricing_manager_->budgetLeft()) {
        int edge = deck[i];
        if (local_search_->worthInvest(edge) &&
            !sol.chosen(edge) && !changed_[edge] &&
            pricing_manager_->consume(pricing_manager_->getCost(edge))) {
            model_update.push_back(id_val(edge,local_search_->getAlternativeConductance(1,edge)));
            new_sol.choose(edge,local_search_->getAlternativeConductance(1,edge));
        }
        i++;
    }
}

void Destroyer::addHighestCurrentEdges(Solution& sol,
                      Solution& new_sol,
                      std::vector<id_val>& model_update) {

    throw std::runtime_error("Not implemented yet");
}

void Destroyer::addBfsEdges(Solution& sol,
                      Solution& new_sol,
                      std::vector<id_val>& model_update) {

    throw std::runtime_error("Not implemented yet");
}

void Destroyer::addBfsRandomEdges(Solution& sol,
                      Solution& new_sol,
                      std::vector<id_val>& model_update) {
    throw std::runtime_error("Not implemented yet");
}
