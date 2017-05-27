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
    PetscErrorCode ierr;
    
    typedef struct edge_pos {
        uint mid; //matrix id
        int row;
        int col;
    } edge_pos;
    //Mappting from edges to the positions they influence in the
    //matrix
    std::vector< std::vector<edge_pos> > e2m;
public:

    SolvItPETSc(std::vector<std::pair<int,int> >& p,
                int* argc, char*** argv, Mode m = MULTI);
    ~SolvItPETSc();

    bool compile();
    
    bool updateConductances(std::vector<ECircuit::EdgeID> e,
                            std::vector<double> v);
    bool solve();

    bool getVoltages(std::vector<double>& sol);
    bool getCurrents(std::vector<double>& c_n,
                     std::vector<double>& c_e);
};

#endif /*SOLVER_USE_PETSC */

#endif /* _SOLVER_ITER_PETSC_HPP_ */
