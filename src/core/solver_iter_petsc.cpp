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
                         Mode m) : Solver(p), mode_(m) {
    argc = *_argc;
    argv = new char*[argc];
    for (int i = 0; i < argc; i++) {
        argv[i] = new char[strlen((*_argv)[i])];
        memcpy(argv[i],(*_argv)[i],strlen((*_argv)[i])*sizeof(char));
    }

}

SolvItPETSc::~SolvItPETSc() {
    for (uint i = 0; i < laplacians_.size(); i++){
        petsc_error_ = MatDestroy(laplacians_[i]); ERROR1(petsc_error_);
        petsc_error_ = VecDestroy(current_flow_[i]); ERROR1(petsc_error_);
        petsc_error_ = VecDestroy(voltages_[i]); ERROR1(petsc_error_);
    }
}

bool SolvItPETSc::compile() {
    if(!Solver::compile()) return false;

    
    std::vector<std::pair<int,int> >& p = focals;
    for (uint i = 0; i < laplacians_.size(); i++){
        petsc_error_ = MatDestroy(laplacians_[i]); ERROR1(petsc_error_);
        petsc_error_ = VecDestroy(current_flow_[i]); ERROR1(petsc_error_);
        petsc_error_ = VecDestroy(voltages_[i]); ERROR1(petsc_error_);
    }
    laplacians_.clear();
    current_flow_.clear();
    voltages_.clear();
    edge2positions_ = std::vector< std::vector<edge_pos> >(nbEdges());
    
    PetscInitialize(&argc,&argv,(char*)0,"PETSc");
    PetscMPIInt size;

    petsc_error_ = MPI_Comm_size(PETSC_COMM_WORLD,&size);
    ERROR(petsc_error_,"Could not retrieve MPI_Comm_size.");
    if (size != 1) {
        ERROR(1,"This is a uniprocessor code only!");
    }

    
    int n = nbNodes();
    int dim = mode_ == MULTI ? n-1 : (n-1) * p.size();
    Mat* lap1 = new Mat();
    Vec* iflow1 = new Vec();
    Vec* volt1 = new Vec();
    petsc_error_ = VecCreate(PETSC_COMM_WORLD,iflow1);ERROR1(petsc_error_);
    petsc_error_ = VecSetSizes(*iflow1,PETSC_DECIDE,dim);ERROR1(petsc_error_);
    petsc_error_ = VecSetFromOptions(*iflow1);ERROR1(petsc_error_);
    petsc_error_ = VecCreate(PETSC_COMM_WORLD,volt1);ERROR1(petsc_error_);
    petsc_error_ = VecSetSizes(*volt1,PETSC_DECIDE,dim);ERROR1(petsc_error_);
    petsc_error_ = VecSetFromOptions(*volt1);ERROR1(petsc_error_);
    petsc_error_ = MatCreate(PETSC_COMM_WORLD,lap1);ERROR1(petsc_error_);
    petsc_error_ = MatSetSizes(*lap1,PETSC_DECIDE,PETSC_DECIDE,dim,dim);ERROR1(petsc_error_);
    petsc_error_ = MatSetFromOptions(*lap1);ERROR1(petsc_error_);
    petsc_error_ = MatSetUp(*lap1);ERROR1(petsc_error_);
    petsc_error_ = MatAssemblyBegin(*lap1,MAT_FINAL_ASSEMBLY);ERROR1(petsc_error_);
    petsc_error_ = MatAssemblyEnd(*lap1,MAT_FINAL_ASSEMBLY);ERROR1(petsc_error_);
    laplacians_.push_back(lap1);
    current_flow_.push_back(iflow1);
    voltages_.push_back(volt1);
    for (uint i = 0; i < p.size(); i++) {
        int s = p[i].first;
        int t = p[i].second;
        int matrix_id = i;
        if (s == t)
            continue;
        if (i > 0 && mode_ == MULTI) {
            Mat* lap1 = new Mat();
            Vec* iflow1 = new Vec();
            Vec* volt1 = new Vec();
            petsc_error_ = VecCreate(PETSC_COMM_WORLD,iflow1);ERROR1(petsc_error_);
            petsc_error_ = VecSetSizes(*iflow1,PETSC_DECIDE,dim);ERROR1(petsc_error_);
            petsc_error_ = VecSetFromOptions(*iflow1);ERROR1(petsc_error_);
            petsc_error_ = VecCreate(PETSC_COMM_WORLD,volt1);ERROR1(petsc_error_);
            petsc_error_ = VecSetSizes(*volt1,PETSC_DECIDE,dim);ERROR1(petsc_error_);
            petsc_error_ = VecSetFromOptions(*volt1);ERROR1(petsc_error_);
            petsc_error_ = MatCreate(PETSC_COMM_WORLD,lap1);ERROR1(petsc_error_);
            petsc_error_ = MatSetSizes(*lap1,PETSC_DECIDE,PETSC_DECIDE,dim,dim);ERROR1(petsc_error_);
            petsc_error_ = MatSetFromOptions(*lap1);ERROR1(petsc_error_);
            petsc_error_ = MatSetUp(*lap1);ERROR1(petsc_error_);
            petsc_error_ = MatAssemblyBegin(*lap1,MAT_FINAL_ASSEMBLY);ERROR1(petsc_error_);
            petsc_error_ = MatAssemblyEnd(*lap1,MAT_FINAL_ASSEMBLY);ERROR1(petsc_error_);
            laplacians_.push_back(lap1);
            current_flow_.push_back(iflow1);
            voltages_.push_back(volt1);
        }
        if (mode_ == UNIQUE)
            matrix_id = 0;
        Mat* lap = laplacians_.back();
        petsc_error_ = MatAssemblyBegin(*lap,MAT_FINAL_ASSEMBLY);ERROR1(petsc_error_);
        Vec* ifl = current_flow_.back();
        int rshift = mode_ == UNIQUE ? i*(n-1) : 0;
        int cshift = mode_ == UNIQUE ? i*(n-1) : 0;

        if (s < t) {
            petsc_error_ = VecSetValue(*ifl,rshift+s,1,INSERT_VALUES);ERROR1(petsc_error_);
        } else {
            petsc_error_ = VecSetValue(*ifl,rshift+s-1,1,INSERT_VALUES);ERROR1(petsc_error_);
        }
        
        for (int e = 0; e < nbEdges(); e++) {
            std::pair<NodeID,NodeID> uv = getNodes(e);
            int u = uv.first;
            int v = uv.second;
            if (u == t || v == t) {
                continue;
            }            
            if (u > t) u--;
            if (v > t) v--;
            int row = rshift+u;
            int col = cshift+v;
            double val = -getConductance(e);
            petsc_error_ = MatSetValues(*lap,1,&row,1,&col,&val,INSERT_VALUES);ERROR1(petsc_error_);
            edge2positions_[e].push_back({matrix_id,row, col});
            row = rshift+v;
            col = cshift+u;
            petsc_error_ = MatSetValues(*lap,1,&row,1,&col,&val,INSERT_VALUES);ERROR1(petsc_error_);
            edge2positions_[e].push_back({matrix_id,row, col});
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
                s += getConductance(e);
                edge2positions_[e].push_back({matrix_id,row, col});
            }
            petsc_error_ = MatSetValues(*lap,1,&row,1,&col,&s,INSERT_VALUES);ERROR1(petsc_error_);
        }
        petsc_error_ = MatAssemblyEnd(*lap,MAT_FINAL_ASSEMBLY);ERROR1(petsc_error_);
        /*
        printf("LAP:\n");
        petsc_error_ = MatView(*lap,PETSC_VIEWER_STDOUT_WORLD); ERROR1(petsc_error_);
        printf("FLOW:\n");
        petsc_error_ = VecView(*ifl,PETSC_VIEWER_STDOUT_WORLD); ERROR1(petsc_error_);
        //*/
    }
    return true;
}

void SolvItPETSc::updateConductances(std::vector<id_val>& ev) {

    for (uint i = 0; i < laplacians_.size(); i++) {
        petsc_error_ = MatAssemblyBegin(*(laplacians_[i]),MAT_FINAL_ASSEMBLY);ERROR1(petsc_error_);
    }
    
    for (uint i = 0; i < ev.size(); i++) {
        int e = ev[i].id;
        double c = ev[i].val;
        for (uint j = 0; j < edge2positions_[e].size(); j++) {
            edge_pos data = edge2positions_[e][j];
            //Erase old cond and add new one.
            //For diag: -old + new
            double val = -getConductance(e) + c;
            if (data.row != data.col) {
                //Offdiag : +old - new
                val = +getConductance(e) - c;
            }
            //std::cout<<"ADD_VALUES "<<val<< " "<<getCond(e)<<" "<<c<<std::endl;
            petsc_error_ = MatSetValues(*(laplacians_[data.matrix_id_]),1,
                                &data.row,1,&data.col,&val,ADD_VALUES);
            ERROR1(petsc_error_);
        }
        updateConductance(e,c);
    }
    

    for (uint i = 0; i < laplacians_.size(); i++) {
        petsc_error_ = MatAssemblyEnd(*(laplacians_[i]),MAT_FINAL_ASSEMBLY);ERROR1(petsc_error_);
    }
    
}
bool SolvItPETSc::solve() {
    for (uint i = 0; i < laplacians_.size(); i++) {
        KSP ksp;         /* linear solver context */
        PC  pc;          /* preconditioner context */
        Mat* lap = laplacians_[i];
        petsc_error_ = KSPCreate(PETSC_COMM_WORLD,&ksp);ERROR1(petsc_error_);
        petsc_error_ = KSPSetOperators(ksp,*lap,*lap);ERROR1(petsc_error_);
        petsc_error_ = KSPGetPC(ksp,&pc);ERROR1(petsc_error_);
        petsc_error_ = PCSetType(pc,PCJACOBI);ERROR1(petsc_error_);
        petsc_error_ = KSPSetTolerances(ksp,1.e-20,PETSC_DEFAULT,PETSC_DEFAULT,PETSC_DEFAULT);ERROR1(petsc_error_);
        petsc_error_ = KSPSetFromOptions(ksp);ERROR1(petsc_error_);
        petsc_error_ = KSPSetType(ksp,KSPCG);ERROR1(petsc_error_);

        PetscScalar p = .5;
        petsc_error_ = VecSet(*voltages_[i],p);CHKERRQ(petsc_error_);
        petsc_error_ = KSPSetInitialGuessNonzero(ksp,PETSC_TRUE);CHKERRQ(petsc_error_);
        petsc_error_ = KSPSolve(ksp,*current_flow_[i],*voltages_[i]);ERROR1(petsc_error_);
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
        if (mode_ == MULTI)
            vi = voltages_[i];
        else
            vi = voltages_[0];
        for (int j = 0; j < nbNodes(); j++) {
            all[j].id = j;
            each[i][j].id = j;
            if (j == t) continue;
            int idx = 0;
            if (j > t)
                idx = j - 1;
            else
                idx = j;
            if (mode_ == UNIQUE)
                idx += (nbNodes() - 1)*i;
            double tmp;
            petsc_error_ = VecGetValues(*vi,1,&idx,&tmp);ERROR1(petsc_error_);
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
        std::pair<NodeID,NodeID> uv = getNodes(e);
        int u = uv.first;
        int v = uv.second;
        c_e[e].id = e;
        c_e[e].val = fabs(vs[u].val - vs[v].val)/getConductance(e);
        c_n[u].id = u;
        c_n[u].val += c_e[e].val/2.0;
        c_n[v].id = v;
        c_n[v].val += c_e[e].val/2.0;
    }
    
}

#endif /*SOLVER_USE_PETSC */
