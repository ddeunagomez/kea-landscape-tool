#ifndef _SOLVER_ITER_PETSC_HPP_
#define _SOLVER_ITER_PETSC_HPP_

#define SOLVER_USE_PETSC
#ifdef SOLVER_USE_PETSC


#include "solver_wrapper.hpp"
#include <unordered_map>
#include <petscksp.h>

/*
 * Solver using PEtSC in iterative methods.
 */
class SolverPetsc : public Solver {

private:
    //Argument options
    int argc;
    char** argv;
    
    std::vector<Mat*> laplacians_;
    std::vector<Vec*> current_flow_;
    std::vector<Vec*> voltages_;
    PetscErrorCode petsc_error_;
    
    typedef struct edge_pos {
        uint matrix_id_; //matrix id
        int row;
        int col;
    } edge_pos;
    //Mappting from edges to the positions they influence in the
    //matrix
    std::vector< std::vector<edge_pos> > edge2positions_;
public:

    SolverPetsc(std::vector<std::pair<int,int> >& p,
                int* argc, char*** argv, MultifocalMatrixMode mode_ = Solver::kOneMatrixPerPair);
    ~SolverPetsc();

    bool compile();

    void updateConductances(std::vector<id_val>& ev);

    bool solve();

    void getVoltages(std::vector< std::vector<id_val> >& each,
                     std::vector<id_val>& all);
    void getCurrents(std::vector<id_val>& c_n,
                     std::vector<id_val>& c_e);
};

#endif /*SOLVER_USE_PETSC */

#endif /* _SOLVER_ITER_PETSC_HPP_ */
