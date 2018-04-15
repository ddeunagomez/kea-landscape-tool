#ifndef _SOLVER_ARMADILLO_HPP_
#define _SOLVER_ARMADILLO_HPP_
#define SOLVER_USE_ARMADILLO
#ifdef SOLVER_USE_ARMADILLO


#include "solver_wrapper.hpp"
#include <unordered_map>
#include <armadillo>


/*
 * Solver using Armadillo. Seems to be much faster than SoPlex
 * since this is not really an LP.
 */
class SolverArmadillo : public Solver {

private:
    std::vector<arma::sp_mat> laplacians_;
    std::vector<arma::vec> current_flow_;
    std::vector<arma::vec> voltages_;

public:

    SolverArmadillo(const std::vector<std::pair<NodeID, NodeID> > &p, MultifocalMatrixMode mode_ = kOneMatrixPerPair);
    ~SolverArmadillo();

    bool compile();
    
    bool solve();

    void getVoltages(std::vector< std::vector<id_val> >& each,
                     std::vector<id_val>& all);

};

#endif /*SOLVER_USE_ARMADILLO */

#endif /* _SOLVER_ARMADILLO_HPP_ */
