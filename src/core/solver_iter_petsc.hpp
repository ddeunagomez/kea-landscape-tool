#ifndef _SOLVER_ITER_PETSC_HPP_
#define _SOLVER_ITER_PETSC_HPP_

#ifdef SOLVER_USE_PETSC


#include "solver_wrapper.hpp"
#include <unordered_map>
#include <petscksp.h>

/*
 * Solver using PEtSC in iterative methods.
 */
class SolvItPETSc : public Solver {

private:
    //Argument options
    int argc;
    char** argv;
    
    std::vector<Mat*> laplacians;
    std::vector<Vec*> iflow;
    std::vector<Vec*> voltages;
    Mode m;
public:

    SolvItPETSc(std::vector<std::pair<int,int> >& p,
                int* argc, char*** argv, Mode m = MULTI);
    ~SolvItPETSc();

    bool compile();
    
    bool updateConductances(std::vector<ECircuit::EdgeID> e,
                            std::vector<double> v);
    bool solve();

    bool getVoltages(std::vector<double>& sol);
};

#endif /*SOLVER_USE_PETSC */

#endif /* _SOLVER_ITER_PETSC_HPP_ */
