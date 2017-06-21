#ifndef DESTROYER_H
#define DESTROYER_H

#include "ecircuit.hpp"
#include "local_search_engine.hpp"

class Destroyer {
public:
    enum RemoveInv {INVRAND, INVLC, INVLCP};
    enum NewInv {WILRAND, WILBFS, WILBFSRAND, WILHC, WILHCP};
private:
    LocalSearchEngine* ls;
    PricingManager* pm;
    RemoveInv invt;
    NewInv newt;
    //Rate is just a counter of edges for now.
    //Maybe crete a mode where its s budget instead?
    int rate;
    int used_rate;
    bool stillAvailableRate() {return used_rate < rate; }
    void removeInv(int e) {
        rate--;
        pm->save(pm->getCost(e));
    }

    //Temporaary state:
    std::vector<bool> changed;
public:
    Destroyer(LocalSearchEngine* ls, PricingManager* pm,
              RemoveInv t1, NewInv t2,
              int rate) :
        ls(ls), pm(pm),invt(t1),newt(t2), rate(rate), used_rate(0) { }
   


    void destroy(Solution& sol,
                 Solution& new_sol,
                 std::vector<id_val>& model_update);

protected:
    //Destroyer functions:
    void destroy_invlc_e(Solution& sol,
                         Solution& new_sol,
                         std::vector<id_val>& model_update);
    void destroy_wilhcp_e(Solution& sol,
                          Solution& new_sol,
                          std::vector<id_val>& model_update);
};

#endif /* DESTROYER_H */
