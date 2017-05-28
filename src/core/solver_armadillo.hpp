#ifndef _SOLVER_ARMADILLO_HPP_
#define _SOLVER_ARMADILLO_HPP_

#ifdef SOLVER_USE_ARMADILLO


#include "solver_wrapper.hpp"
#include <unordered_map>
#include <armadillo>


/*
 * Solver using Armadillo. Seems to be much faster than SoPlex
 * since this is not really an LP.
 */
class SolvArmadillo : public Solver {

private:
    std::vector<arma::sp_mat> laplacians;
    std::vector<arma::vec> iflow;
    std::vector<arma::vec> voltages;
    
    Mode m;
public:

    SolvArmadillo(std::vector<std::pair<int,int> >& p, Mode m = MULTI);
    ~SolvArmadillo();

    bool compile();
    
    bool solve();

    void getVoltages(std::vector< std::vector<id_val> >& each,
                     std::vector<id_val>& all);

};

#endif /*SOLVER_USE_ARMADILLO */

#endif /* _SOLVER_ARMADILLO_HPP_ */
