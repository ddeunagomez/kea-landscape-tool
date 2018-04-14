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
    struct RowColumn {
        int row;
        int col;
    };
    std::vector<soplex::SoPlex> soplex_solvers_;
    std::unordered_map<ECircuit::EdgeID, std::vector<RowColumn> > edge_positions_;
    Mode mode_;
public:

    SolvSoplex(std::vector<std::pair<int,int> >& p, Mode mode_ = MULTI);
    ~SolvSoplex();

    bool compile();
    
    bool solve();
    void getVoltages(std::vector< std::vector<id_val> >& each,
                     std::vector<id_val>& all);
};

#endif /*SOLVER_USE_SOPLEX */

#endif /* _SOLVER_SOPLEX_HPP_ */
