#ifdef SOLVER_USE_PETSC

#include "solver_iter_petsc.hpp"
#include "utils.hpp"
#include <math.h>

#define ERROR(c,str) do { if (c) {                                  \
        std::cerr<<str<<" "<<__FILE__<<":"<<__LINE__<<std::endl;   \
        exit(1);                                                \
        } } while(0)
#define ERROR1(c) ERROR(c,"Error: ")

SolvItPETSc::SolvItPETSc(ECircuit& ec, std::vector<std::pair<int,int> >& p,
                         int* argc, char*** argv,
                             Mode m) : Solver(ec,p) {

    PetscInitialize(argc,argv,(char*)0,"PETSc");
    PetscMPIInt size;
    PetscErrorCode ierr;
    ierr = MPI_Comm_size(PETSC_COMM_WORLD,&size);
    ERROR(ierr,"Could not retrieve MPI_Comm_size.");
    if (size != 1) {
        
        std::cerr<<"This is a uniprocessor code only!"<<std::endl;
        exit(1);
    }

    
    int n = ec.nbNodes();
    int dim = m == MULTI ? n-1 : (n-1) * p.size();
    Mat* lap1 = new Mat();
    Vec* iflow1 = new Vec();
    Vec* volt1 = new Vec();
    ierr = VecCreate(PETSC_COMM_WORLD,iflow1);ERROR1(ierr);
    ierr = VecSetSizes(*iflow1,PETSC_DECIDE,dim);ERROR1(ierr);
    ierr = VecSetFromOptions(*iflow1);ERROR1(ierr);
    ierr = VecCreate(PETSC_COMM_WORLD,volt1);ERROR1(ierr);
    ierr = VecSetSizes(*volt1,PETSC_DECIDE,dim);ERROR1(ierr);
    ierr = VecSetFromOptions(*volt1);ERROR1(ierr);
    ierr = MatCreate(PETSC_COMM_WORLD,lap1);ERROR1(ierr);
    ierr = MatSetSizes(*lap1,PETSC_DECIDE,PETSC_DECIDE,dim,dim);ERROR1(ierr);
    ierr = MatSetFromOptions(*lap1);ERROR1(ierr);
    ierr = MatSetUp(*lap1);ERROR1(ierr);
    ierr = MatAssemblyBegin(*lap1,MAT_FINAL_ASSEMBLY);ERROR1(ierr);
    ierr = MatAssemblyEnd(*lap1,MAT_FINAL_ASSEMBLY);ERROR1(ierr);
    laplacians.push_back(lap1);
    iflow.push_back(iflow1);
    voltages.push_back(volt1);
    for (uint i = 0; i < p.size(); i++) {
        int s = p[i].first;
        int t = p[i].second;
        if (s == t)
            continue;
        if (i > 0 && m == MULTI) {
            Mat* lap1 = new Mat();
            Vec* iflow1 = new Vec();
            Vec* volt1 = new Vec();
            ierr = VecCreate(PETSC_COMM_WORLD,iflow1);ERROR1(ierr);
            ierr = VecSetSizes(*iflow1,PETSC_DECIDE,dim);ERROR1(ierr);
            ierr = VecSetFromOptions(*iflow1);ERROR1(ierr);
            ierr = VecCreate(PETSC_COMM_WORLD,volt1);ERROR1(ierr);
            ierr = VecSetSizes(*volt1,PETSC_DECIDE,dim);ERROR1(ierr);
            ierr = VecSetFromOptions(*volt1);ERROR1(ierr);
            ierr = MatCreate(PETSC_COMM_WORLD,lap1);ERROR1(ierr);
            ierr = MatSetSizes(*lap1,PETSC_DECIDE,PETSC_DECIDE,dim,dim);ERROR1(ierr);
            ierr = MatSetFromOptions(*lap1);ERROR1(ierr);
            ierr = MatSetUp(*lap1);ERROR1(ierr);
            ierr = MatAssemblyBegin(*lap1,MAT_FINAL_ASSEMBLY);ERROR1(ierr);
            ierr = MatAssemblyEnd(*lap1,MAT_FINAL_ASSEMBLY);ERROR1(ierr);
            laplacians.push_back(lap1);
            iflow.push_back(iflow1);
            voltages.push_back(volt1);            
        }
        Mat* lap = laplacians.back();
        //Delay constructing Mat object
        //So you only need to assemble once
        double mat[dim][dim]; 
        memset(mat,0,sizeof(double)*dim*dim);
        Vec* ifl = iflow.back();
        int rshift = m == UNIQUE ? i*(n-1) : 0;
        int cshift = m == UNIQUE ? i*(n-1) : 0;

        if (s < t) {
            ierr = VecSetValue(*ifl,rshift+s,1,INSERT_VALUES);ERROR1(ierr);
        } else {
            ierr = VecSetValue(*ifl,rshift+s-1,1,INSERT_VALUES);ERROR1(ierr);
        }
        
        for (int e = 0; e < ec.nbEdges(); e++) {
            int u = ec.getU(e);
            int v = ec.getV(e);
            if (u == t || v == t) {
                continue;
            }            
            if (u > t) u--;
            if (v > t) v--;
            mat[rshift+u][cshift+v] = mat[rshift+u][cshift+v]-ec.getCond(e);
            mat[rshift+v][cshift+u] = mat[rshift+v][cshift+u]-ec.getCond(e);
        }

        for (int j = 0; j < n; j++) {
            if (j == t) continue;
            int rj = j;
            if (j > t)
                rj = j - 1;
            double s = 0;
            for (int k = 0; k < ec.nbEdges(rj); k++) {
                ECircuit::EdgeID e = ec.getEdgeFrom(rj,k);
                s += ec.getCond(e);
            }
            mat[rshift+rj][cshift+rj] = s;
        }

        for (int row = 0; row < dim; row++) {
            for (int col = 0; col < dim; col++) {
                if (mat[row][col] != 0.0) {
                    ierr = MatSetValues(*lap,1,&row,1,&col,&mat[row][col],
                                        INSERT_VALUES);
                    ERROR1(ierr);
                }
            }
        }
        ierr = MatAssemblyBegin(*lap,MAT_FINAL_ASSEMBLY);ERROR1(ierr);
        ierr = MatAssemblyEnd(*lap,MAT_FINAL_ASSEMBLY);ERROR1(ierr);
        /*
        printf("LAP:\n");
        ierr = MatView(*lap,PETSC_VIEWER_STDOUT_WORLD); ERROR1(ierr);
        printf("FLOW:\n");
        ierr = VecView(*ifl,PETSC_VIEWER_STDOUT_WORLD); ERROR1(ierr);
        */

    }
}

SolvItPETSc::~SolvItPETSc() {

}

bool SolvItPETSc::updateConductance(ECircuit::EdgeID e, double v) {
    UNSUPPORTED;
    //TODO!!!
    //Example:
    /*int rows = {0};
    int cols = {0};
    double vals;
    ierr = MatGetValues(*lap,1,&rows,1,&cols,&vals);
    ierr = MatAssemblyBegin(*lap,MAT_FINAL_ASSEMBLY);ERROR1(ierr);
    ierr = MatSetValues(*lap,1,&rows,1,&cols,&vals,
                        INSERT_VALUES);
    ERROR1(ierr);
    ierr = MatAssemblyEnd(*lap,MAT_FINAL_ASSEMBLY);ERROR1(ierr);
    //*/
    return true;
}
bool SolvItPETSc::solve() {
    for (uint i = 0; i < laplacians.size(); i++) {
        KSP ksp;         /* linear solver context */
        PC  pc;          /* preconditioner context */
        PetscErrorCode ierr;
        Mat* lap = laplacians[i];
        ierr = KSPCreate(PETSC_COMM_WORLD,&ksp);ERROR1(ierr);        
        ierr = KSPSetOperators(ksp,*lap,*lap);ERROR1(ierr);
        ierr = KSPGetPC(ksp,&pc);ERROR1(ierr);
        ierr = PCSetType(pc,PCJACOBI);ERROR1(ierr);
        ierr = KSPSetTolerances(ksp,1.e-20,1.e-20,PETSC_DEFAULT,PETSC_DEFAULT);ERROR1(ierr);
        ierr = KSPSetFromOptions(ksp);ERROR1(ierr);
        ierr = KSPSetType(ksp,KSPCG);ERROR1(ierr);

        PetscScalar p = .5;
        ierr = VecSet(*voltages[i],p);CHKERRQ(ierr);
        ierr = KSPSetInitialGuessNonzero(ksp,PETSC_TRUE);CHKERRQ(ierr);
        ierr = KSPSolve(ksp,*iflow[i],*voltages[i]);ERROR1(ierr);
        ierr = VecView(*voltages[i],PETSC_VIEWER_STDOUT_WORLD); ERROR1(ierr);
        exit(0);
    }
    return true;
}


//Voltages indexed by node ids
bool SolvItPETSc::getVoltages(std::vector<double>& sol) {
    sol = std::vector<double>(ec.nbNodes(),0);
    PetscErrorCode ierr;
    for (uint i = 0; i < focals.size(); i++) {
        int s = focals[i].first;
        int t = focals[i].second;
        if (s == t)
            continue;

        Vec* vi = voltages[i];
        for (int j = 0; j < ec.nbNodes(); j++) {
            if (j == t) continue;
            int idx = 0;
            if (j > t)
                idx = j - 1;
            else
                idx = j;
            double tmp;
            ierr = VecGetValues(*vi,1,&idx,&tmp);ERROR1(ierr);
            sol[j] += tmp; 
        }
        
    }
    
    return true;
}

#endif /*SOLVER_USE_PETSC */
