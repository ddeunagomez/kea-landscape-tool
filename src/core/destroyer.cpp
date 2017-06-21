#include "destroyer.hpp"
#include <algorithm>

void Destroyer::destroy(Solution& sol,
                        Solution& new_sol,
                        std::vector<id_val>& model_update) {

    used_rate = 0;
    changed = std::vector<bool>(sol.currents_e.size(),false);

    destroy_invlc_e(sol,new_sol,model_update);
    destroy_wilhcp_e(sol,new_sol,model_update);
    
    switch(invt) {
    case INVRAND:
        break;
    case INVLC:
        break;
    case INVLCP:
        break;
    default:
        throw std::runtime_error("Unknown LP destroyer for RemoveInv");
    }
    


    if (sol == new_sol) {
        std::cout<<"WARNING: Stuck in local minimum. Getting out now."
                 <<std::endl;;
        // Call a probabilistic method to get out of local minimum
        destroy_invlc_e(sol,new_sol,model_update);
        destroy_wilhcp_e(sol,new_sol,model_update);
    }
    
}


void Destroyer::destroy_invlc_e(Solution& sol,
                                Solution& new_sol,
                                std::vector<id_val>& model_update) {

    std::sort(sol.currents_e.begin(), sol.currents_e.end(), id_val::sort_by_val);

    uint i = 0;
    while (i < sol.currents_e.size() && stillAvailableRate()) {
        int e = sol.currents_e[i].id;
        if (sol.chosen(e)) {
            removeInv(e);      //Account edge e into rate
            changed[e] = true; //Do not invest on e, I just disinvested!
            model_update.push_back(id_val(e,ls->getAlt(0,e))); //Set to original
            //std::cout<<"Alternative "<<model_update.back().id
            //<<" "<<model_update.back().val<<std::endl;
            new_sol.discard(e);
        }
        i++;
    }

}


void Destroyer::destroy_wilhcp_e(Solution& sol,
                                 Solution& new_sol,
                                 std::vector<id_val>& model_update) {
    float max_curr = 0.0;
    int nb_edges = sol.currents_e.size();
    for (int i = 0; i < nb_edges; i++) {
        max_curr = MAX(max_curr,sol.currents_e[i].val);
    }
    float threshold = max_curr/(10.0*ls->nbFocals()); //TODO: option for this
    std::vector<id_val> probabilities;
    int low = 0, high = 0;
    int prob_size = 0;
    for (int e = 0; e < nb_edges; e++) {
        if (sol.currents_e[e].val < threshold) low++;	
        else high++;
        probabilities.push_back(id_val(-1,-1));
    }
    assert(high > 0);
    double gambling_budget = 0;
    for (int e = 0; e < nb_edges; e++) {
        if(sol.currents_e[e].val < threshold)
            continue;
        if (!ls->worthInvest(e))
            continue;
        if (sol.chosen(e))
            continue;
        if (changed[e])
            continue;
        float ecurr = sol.currents_e[prob_size].val;
        if (e == 0)
            probabilities[prob_size].val = ecurr;
        else
            probabilities[prob_size].val = ecurr
                + probabilities[prob_size - 1].val;
        probabilities[prob_size].id = e;
        gambling_budget += pm->getCost(e);
        prob_size++;
    }

    //I can buy more than I selected
    if (gambling_budget < pm->budgetLeft()){
        std::vector<int> deck(nb_edges);
        std::iota(deck.begin(), deck.end(),0);
        std::random_shuffle(deck.begin(),deck.end());
        for (int i = 0;
             i < nb_edges && gambling_budget <= pm->budgetLeft();
             i++) {
            int e = deck[i];
            if (!ls->worthInvest(e))
                continue;
            if (sol.chosen(e))
                continue;
            if (changed[e])
                continue;
            float ecurr = sol.currents_e[e].val;
            if(sol.currents_e[e].val < threshold) {
                if (prob_size == 0)
                    probabilities[prob_size].val = ecurr;
                else
                    probabilities[prob_size].val = ecurr
                        + probabilities[prob_size-1].val;
                probabilities[prob_size].id = e;
                gambling_budget += pm->getCost(e);
                prob_size++;
            }
        }
        //assert(prob_size == count);
    } 

    assert(gambling_budget >= pm->budgetLeft());

    //In case I hit exactly the budget:
    if (gambling_budget == pm->budgetLeft()) {
        for (int i = 0; i < prob_size; i++) {
            int e = probabilities[i].id;
            model_update.push_back(id_val(e,ls->getAlt(1,e))); 
            new_sol.choose(e,ls->getAlt(1,e));
        }
        return;
    }

    //General case:
    int last = prob_size - 1;
    while(pm->budgetLeft() > 0 && prob_size > 0) {

        double val = probabilities[last].val
            * (double)rand() / (double)(RAND_MAX);
        int lo = 0; int hi = prob_size - 1;
        int mid = lo + (hi - lo)/2;
        //Dicho search for the edge with that value
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
        
        int check = 0;
        while (probabilities[mid].id == -1) {
            ++mid; 
            mid %= prob_size;
            check++;
            if (check >= 2*prob_size) //Looped twice: nothing left to select!
                return;
            assert(check < 2* prob_size);
        }
        int edge = probabilities[mid].id;
        probabilities[mid].id = -1; //Erase edge
        prob_size--;
        if (mid >= last) last = mid - 1; //For scalling correctly the prob
    	if (pm->consume(pm->getCost(edge))) {
            model_update.push_back(id_val(edge,ls->getAlt(1,edge)));
            new_sol.choose(edge,ls->getAlt(1,edge));
    	}
    }
    
}
