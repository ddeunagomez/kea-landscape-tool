#ifdef SOLVER_USE_ARMADILLO

#include "solver_armadillo.hpp"
#include "utils.hpp"

using namespace arma;

SolvArmadillo::SolvArmadillo(ECircuit& ec, std::vector<std::pair<int,int> >& p, Mode m) : Solver(ec,p) {

    int n = ec.nbNodes();
    int dim = m == MULTI ? n : n * p.size();
    laplacians.push_back(sp_mat(dim,dim));
    iflow.push_back(zeros<vec>(dim));
    voltages.push_back(vec(dim));
    for (uint i = 0; i < p.size(); i++) {
        if (i > 0 && m == MULTI) {
            laplacians.push_back(sp_mat(n,n));
            iflow.push_back(zeros<vec>(n));
            voltages.push_back(vec(n));
        }
        sp_mat& lap = laplacians.back();        
        int s = p[i].first;
        int t = p[i].second;

        int rshift = m == UNIQUE ? i*n : 0;
        int cshift = m == UNIQUE ? i*n : 0;

        iflow.back()[rshift+s] = 1;
        
        for (int e = 0; e < ec.nbEdges(); e++) {
            int u = ec.getU(e);
            int v = ec.getV(e);
            if (u == t || v == t) {
                continue;
            }
            lap(rshift+u, cshift+v) = lap(rshift+u, cshift+v)-ec.getCond(e);
            lap(rshift+v, cshift+u) = lap(rshift+v, cshift+u)-ec.getCond(e);
        }
        for (int i = 0; i < ec.nbNodes(); i++) {
            if (i == t) continue;
            double s = 0;
            for (int j = 0; j < ec.nbEdges(i); j++) {
                ECircuit::EdgeID e = ec.getEdgeFrom(i,j);
                s += ec.getCond(e);
            }
            lap(rshift+i, cshift+i) = s;
        }
        
        //*
        mat laptmp(lap);
        laptmp.print();
        iflow.back().print();
        std::cout<<std::endl;
        //*/
    }
    
}

SolvArmadillo::~SolvArmadillo() {

}

bool SolvArmadillo::updateConductance(ECircuit::EdgeID e, double v) {
    UNSUPPORTED;
    return true;
}
bool SolvArmadillo::solve() {
    for (uint i = 0; i < laplacians.size(); i++) {
        sp_mat laptmp(laplacians[i].n_rows - 1, laplacians[i].n_cols - 1);
        for (int j = 0; j < 4; j++)
            for (int k = 0; k < 4; k++)
                laptmp(j,k) = laplacians[i](j,k);
        vec iftmp(4);
        for (int j = 0; j < 4; j++)
            iftmp(j) = iflow[i](j);
        //std::cout<<"Det: "<<det(laptmp)<<std::endl;
        voltages[i] = spsolve(laptmp,iftmp,"lapack");
        for (int j = 0; j < voltages[i].n_rows; j++)
            std::cout<<"v("<<j<<") = "<<voltages[i](j)<<std::endl;
    }
    return true;
}

//Currents indexed by node ids
bool SolvArmadillo::getCurrents(std::vector<double>& sol) {
    UNSUPPORTED;
    return true;
}

#endif /*SOLVER_USE_ARMADILLO */
