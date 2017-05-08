#ifdef SOLVER_USE_ARMADILLO

#include "solver_armadillo.hpp"
#include "utils.hpp"

SolvArmadillo::SolvArmadillo(ECircuit& ec, std::vector<std::pair<int,int> >& p, Mode m) : Solver(ec,p) {

}

SolvArmadillo::~SolvArmadillo() {

}

bool SolvArmadillo::updateConductance(ECircuit::EdgeID e, double v) {
    UNSUPPORTED;
    return true;
}
bool SolvArmadillo::solve() {
    UNSUPPORTED;
    return true;
}

//Currents indexed by node ids
bool SolvArmadillo::getCurrents(std::vector<double>& sol) {
    UNSUPPORTED;
    return true;
}

#endif /*SOLVER_USE_ARMADILLO */
