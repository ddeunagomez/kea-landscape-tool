#ifdef SOLVER_USE_PETSC

#include "solver_iter_petsc.hpp"
#include "utils.hpp"
#include <math.h>
#include <string>

#define ERROR(c,str) do { if (c) {                                      \
            std::cerr<<str<<" "<<__FILE__<<":"<<__LINE__<<std::endl;    \
            std::string err = str;                                      \
            err += " ";                                                 \
            err += __FILE__;                                            \
            err += ":";                                                 \
            err += __LINE__;                                            \
            throw std::runtime_error(err);                              \
        } } while(0)
#define ERROR1(c) ERROR(c,"PETSc error: ")

SolvItPETSc::SolvItPETSc(std::vector<std::pair<int,int> >& p,
                         int* _argc, char*** _argv,
                         Mode m) : Solver(p), m(m) {
    argc = *_argc;
    argv = new char*[argc];
    for (int i = 0; i < argc; i++) {
        argv[i] = new char[strlen((*_argv)[i])];
        memcpy(argv[i],(*_argv)[i],strlen((*_argv)[i])*sizeof(char));
    }

}

SolvItPETSc::~SolvItPETSc() {
    for (uint i = 0; i < laplacians.size(); i++){
        ierr = MatDestroy(laplacians[i]); ERROR1(ierr);
        ierr = VecDestroy(iflow[i]); ERROR1(ierr);
        ierr = VecDestroy(voltages[i]); ERROR1(ierr);
    }
}

bool SolvItPETSc::compile() {
    if(!Solver::compile()) return false;

    if (m == MULTI)
        throw std::runtime_error("Cannot retrieve voltages for MULTI in PETSc yet.");
    
    std::vector<std::pair<int,int> >& p = focals;
    for (uint i = 0; i < laplacians.size(); i++){
        ierr = MatDestroy(laplacians[i]); ERROR1(ierr);
        ierr = VecDestroy(iflow[i]); ERROR1(ierr);
        ierr = VecDestroy(voltages[i]); ERROR1(ierr);
    }
    laplacians.clear();
    iflow.clear();
    voltages.clear();
    e2m = std::vector< std::vector<edge_pos> >(nbEdges());
    
    PetscInitialize(&argc,&argv,(char*)0,"PETSc");
    PetscMPIInt size;

    ierr = MPI_Comm_size(PETSC_COMM_WORLD,&size);
    ERROR(ierr,"Could not retrieve MPI_Comm_size.");
    if (size != 1) {
        ERROR(1,"This is a uniprocessor code only!");
    }

    
    int n = nbNodes();
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
        int matrix_id = i;
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
        if (m == UNIQUE)
            matrix_id = 0;
        Mat* lap = laplacians.back();
        ierr = MatAssemblyBegin(*lap,MAT_FINAL_ASSEMBLY);ERROR1(ierr);
        Vec* ifl = iflow.back();
        int rshift = m == UNIQUE ? i*(n-1) : 0;
        int cshift = m == UNIQUE ? i*(n-1) : 0;

        if (s < t) {
            ierr = VecSetValue(*ifl,rshift+s,1,INSERT_VALUES);ERROR1(ierr);
        } else {
            ierr = VecSetValue(*ifl,rshift+s-1,1,INSERT_VALUES);ERROR1(ierr);
        }
        
        for (int e = 0; e < nbEdges(); e++) {
            int u = getU(e);
            int v = getV(e);
            if (u == t || v == t) {
                continue;
            }            
            if (u > t) u--;
            if (v > t) v--;
            int row = rshift+u;
            int col = cshift+v;
            double val = -getCond(e);
            ierr = MatSetValues(*lap,1,&row,1,&col,&val,INSERT_VALUES);ERROR1(ierr);
            e2m[e].push_back({matrix_id,row, col});
            assert(matrix_id == 0);
            row = rshift+v;
            col = cshift+u;
            ierr = MatSetValues(*lap,1,&row,1,&col,&val,INSERT_VALUES);ERROR1(ierr);
            e2m[e].push_back({matrix_id,row, col});
            assert(matrix_id == 0);
        }
        for (int j = 0; j < n; j++) {
            if (j == t) continue;
            int rj = j;
            if (j > t)
                rj = j - 1;
            double s = 0;
            int row = rshift+rj;
            int col = cshift+rj;
            for (int k = 0; k < nbEdges(j); k++) {
                ECircuit::EdgeID e = getEdgeFrom(j,k);
                s += getCond(e);
                e2m[e].push_back({matrix_id,row, col});
                assert(matrix_id == 0);
            }
            ierr = MatSetValues(*lap,1,&row,1,&col,&s,INSERT_VALUES);ERROR1(ierr);
        }
        ierr = MatAssemblyEnd(*lap1,MAT_FINAL_ASSEMBLY);ERROR1(ierr);
        /*
        printf("LAP:\n");
        ierr = MatView(*lap,PETSC_VIEWER_STDOUT_WORLD); ERROR1(ierr);
        printf("FLOW:\n");
        ierr = VecView(*ifl,PETSC_VIEWER_STDOUT_WORLD); ERROR1(ierr);
        //*/
    }
    return true;
}

void SolvItPETSc::updateConductances(std::vector<ECircuit::EdgeID> edges,
                                     std::vector<double> vals) {

    for (uint i = 0; i < laplacians.size(); i++) {
        ierr = MatAssemblyBegin(*(laplacians[i]),MAT_FINAL_ASSEMBLY);ERROR1(ierr);
    }
    
    for (uint i = 0; i < edges.size(); i++) {
        int e = edges[i];
        int c = vals[i];
        for (uint j = 0; j < e2m[e].size(); j++) {
            edge_pos data = e2m[e][j];
            //Erase old cond and add new one.
            //For diag: -old + new
            double val = -getCond(e) + c;
            if (data.row != data.col) {
                //Offdiag : +old - new
                val = +getCond(e) - c;
            }
            ierr = MatSetValues(*(laplacians[data.mid]),1,
                                &data.row,1,&data.col,&val,ADD_VALUES);
            ERROR1(ierr);
        }
        updateCond(e,c);
    }
    

    for (uint i = 0; i < laplacians.size(); i++) {
        ierr = MatAssemblyEnd(*(laplacians[i]),MAT_FINAL_ASSEMBLY);ERROR1(ierr);
    }
    
}
bool SolvItPETSc::solve() {
    for (uint i = 0; i < laplacians.size(); i++) {
        KSP ksp;         /* linear solver context */
        PC  pc;          /* preconditioner context */
        Mat* lap = laplacians[i];
        ierr = KSPCreate(PETSC_COMM_WORLD,&ksp);ERROR1(ierr);        
        ierr = KSPSetOperators(ksp,*lap,*lap);ERROR1(ierr);
        ierr = KSPGetPC(ksp,&pc);ERROR1(ierr);
        ierr = PCSetType(pc,PCJACOBI);ERROR1(ierr);
        ierr = KSPSetTolerances(ksp,1.e-20,PETSC_DEFAULT,PETSC_DEFAULT,PETSC_DEFAULT);ERROR1(ierr);
        ierr = KSPSetFromOptions(ksp);ERROR1(ierr);
        ierr = KSPSetType(ksp,KSPCG);ERROR1(ierr);

        PetscScalar p = .5;
        ierr = VecSet(*voltages[i],p);CHKERRQ(ierr);
        ierr = KSPSetInitialGuessNonzero(ksp,PETSC_TRUE);CHKERRQ(ierr);
        ierr = KSPSolve(ksp,*iflow[i],*voltages[i]);ERROR1(ierr);
    }
    return true;
}


//Voltages indexed by node ids
void SolvItPETSc::getVoltages(std::vector< std::vector<id_val> >& each,
                              std::vector<id_val>& all) {
    all = std::vector<id_val>(nbNodes());
    each = std::vector< std::vector<id_val> >(focals.size(),
                                              std::vector<id_val>(nbNodes()));

    for (uint i = 0; i < focals.size(); i++) {
        int s = focals[i].first;
        int t = focals[i].second;
        if (s == t)
            continue;

        Vec* vi = NULL;
        if (m == MULTI)
            vi = voltages[i];
        else
            vi = voltages[0];
        for (int j = 0; j < nbNodes(); j++) {
            all[j].id = j;
            each[i][j].id = j;
            if (j == t) continue;
            int idx = 0;
            if (j > t)
                idx = j - 1;
            else
                idx = j;
            if (m == UNIQUE)
                idx += (nbNodes() - 1)*i;
            double tmp;
            ierr = VecGetValues(*vi,1,&idx,&tmp);ERROR1(ierr);
            all[j].val += tmp;
            each[i][j].val += tmp;
        }
        
    }
    
}

void SolvItPETSc::getCurrents(std::vector<id_val>& c_n,
                              std::vector<id_val>& c_e) {

    std::vector<id_val> vs;
    std::vector< std::vector<id_val> > dummy;
    getVoltages(dummy, vs);
    c_n = std::vector<id_val>(nbNodes());
    c_e = std::vector<id_val>(nbEdges());
    
    for (int e = 0; e < nbEdges(); e++) {
        int u = getU(e);
        int v = getV(e);
        c_e[e].id = e;
        c_e[e].val = fabs(vs[u].val - vs[v].val)/getCond(e);
        c_n[u].id = u;
        c_n[u].val += c_e[e].val/2.0;
        c_n[v].id = v;
        c_n[v].val += c_e[e].val/2.0;
    }
    
}

#endif /*SOLVER_USE_PETSC */
