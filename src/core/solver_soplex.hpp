#ifndef _SOLVER_SOPLEX_HPP_
#define _SOLVER_SOPLEX_HPP_

#ifdef SOLVER_USE_SOPLEX


#include "solver_wrapper.hpp"
#include "soplex.h"
#include <unordered_map>

/*
 * This class will build a SoPlex instance for each pair of focals
 * I'll try to put all in one unique instance, and see which is faster:
 * plenty of small probems, or one big problem.
 */
class SolvSoplex : public Solver {

private:
    struct RC {
        int row;
        int col;
    };
    std::vector<soplex::SoPlex> solvers;
    std::unordered_map<ECircuit::EdgeID, std::vector<RC> > edge_pos;
    Mode m;
public:

    SolvSoplex(ECircuit& ec, std::vector<std::pair<int,int> >& p, Mode m = MULTI);
    ~SolvSoplex();

    bool updateConductance(ECircuit::EdgeID e, double v);
    bool solve();
    bool getVoltages(std::vector<double>& sol);

};

#endif /*SOLVER_USE_SOPLEX */

#endif /* _SOLVER_SOPLEX_HPP_ */
