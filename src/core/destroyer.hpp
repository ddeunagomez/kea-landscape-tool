#ifndef DESTROYER_H
#define DESTROYER_H

#include "ecircuit.hpp"
#include "local_search_engine.hpp"

class Destroyer {
public:
    enum RemoveInvetment {INVRAND, INVLC, INVLCP};
    enum NewInvestment {WILRAND, WILBFS, WILHC, WILHCP};
private:
    LocalSearchEngine* local_search_;
    PricingManager* pricing_manager_;
    RemoveInvetment remove_strategy_;
    NewInvestment add_strategy_;
    //Rate is just a counter of edges for now.
    //Maybe crete a mode where its s budget instead?
    int destruction_rate_;
    int consumed_rate_;
    bool hasAvailableRate() {return consumed_rate_ < destruction_rate_; }
    void removeInvestment(int e) {
        destruction_rate_--;
        pricing_manager_->save(pricing_manager_->getCost(e));
    }

    //Temporaary state:
    std::vector<bool> changed_;

    float getMaxCurrent(Solution& sol);
public:
    Destroyer(LocalSearchEngine* ls, PricingManager* pm,
              RemoveInvetment t1, NewInvestment t2,
              int rate) :
        local_search_(ls), pricing_manager_(pm),remove_strategy_(t1),add_strategy_(t2), destruction_rate_(rate), consumed_rate_(0) { }
   


    void destroy(Solution& sol,
                 Solution& new_sol,
                 std::vector<id_val>& model_update);

protected:
    //Destroyer functions:
    void destroyLeastCurrentEdges(Solution& sol,
                         Solution& new_sol,
                         std::vector<id_val>& model_update);
    void addWildEdgesHighCurrentProb(Solution& sol,
                          Solution& new_sol,
                          std::vector<id_val>& model_update);
};

#endif /* DESTROYER_H */
