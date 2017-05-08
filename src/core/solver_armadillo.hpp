#ifndef _SOLVER_ARMADILLO_HPP_
#define _SOLVER_ARMADILLO_HPP_

#ifdef SOLVER_USE_ARMADILLO


#include "solver_wrapper.hpp"
#include <unordered_map>

/*
 * Solver using Armadillo. Seems to be much faster than SoPlex
 * since this is not really an LP.
 */
class SolvArmadillo : public Solver {

private:
    Mode m;
public:

    SolvArmadillo(ECircuit& ec, std::vector<std::pair<int,int> >& p, Mode m = MULTI);
    ~SolvArmadillo();

    bool updateConductance(ECircuit::EdgeID e, double v);
    bool solve();

    //Currents indexed by node ids
    bool getCurrents(std::vector<double>& sol);

};

#endif /*SOLVER_USE_ARMADILLO */

#endif /* _SOLVER_ARMADILLO_HPP_ */
