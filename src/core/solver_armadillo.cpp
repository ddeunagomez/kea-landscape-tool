#ifdef SOLVER_USE_ARMADILLO

#include "solver_armadillo.hpp"
#include "utils.hpp"
#include <math.h>

using namespace arma;

SolvArmadillo::SolvArmadillo(std::vector<std::pair<int,int> >& p,
                             Mode m) : Solver(p), m(m) {

   
}

SolvArmadillo::~SolvArmadillo() {

}

bool SolvArmadillo::compile() {
    if(!Solver::compile()) return false;

    if (m == MULTI)
        throw std::runtime_error("Cannot retrieve voltages for MULTI in Armadillo yet.");
    
    std::vector<std::pair<int,int> >& p = focals;
    laplacians.clear();
    iflow.clear();
    voltages.clear();

    int n = nbNodes();
    int dim = m == MULTI ? n-1 : (n-1) * p.size();
    laplacians.push_back(sp_mat(dim,dim));
    iflow.push_back(zeros<vec>(dim));
    voltages.push_back(vec(dim));
    for (uint i = 0; i < p.size(); i++) {
        int s = p[i].first;
        int t = p[i].second;
        if (s == t)
            continue;
        if (i > 0 && m == MULTI) {
            laplacians.push_back(sp_mat(n - 1,n - 1));
            iflow.push_back(zeros<vec>(n - 1));
            voltages.push_back(vec(n - 1));
        }
        sp_mat& lap = laplacians.back();        

        int rshift = m == UNIQUE ? i*(n-1) : 0;
        int cshift = m == UNIQUE ? i*(n-1) : 0;

        if (s < t)
            iflow.back()[rshift+s] = 1;
        else
            iflow.back()[rshift+s - 1] = 1;
        
        for (int e = 0; e < nbEdges(); e++) {
            int u = getU(e);
            int v = getV(e);
            if (u == t || v == t) {
                continue;
            }            
            if (u > t) u--;
            if (v > t) v--;
            //std::cout<<"Access ("<<rshift+u<<","<<cshift+v<<")"<<std::endl;
            //std::cout<<"Access ("<<rshift+v<<","<<cshift+u<<")"<<std::endl;
            lap(rshift+u, cshift+v) = lap(rshift+u, cshift+v)-getCond(e);
            lap(rshift+v, cshift+u) = lap(rshift+v, cshift+u)-getCond(e);
        }
        //std::cout<<"Done "<<t<<" "<<rshift<<" "<<cshift<<std::endl;

        for (int j = 0; j < n; j++) {
            if (j == t) continue;
            int rj = j;
            if (j > t)
                rj = j - 1;
            double s = 0;
            for (int k = 0; k < nbEdges(j); k++) {
                ECircuit::EdgeID e = getEdgeFrom(j,k);
                s += getCond(e);
            }
            //std::cout<<"Diag ("<<rshift+rj<<","<<cshift+rj<<")"<<std::endl;
            lap(rshift+rj, cshift+rj) = s;
        }
        //std::cout<<"Done"<<std::endl;
        /*
        mat laptmp(lap);
        laptmp.print();
        iflow.back().print();
        std::cout<<std::endl;
        //*/
    }
    return true;
}


bool SolvArmadillo::solve() {
    for (uint i = 0; i < laplacians.size(); i++) {
        voltages[i] = spsolve(laplacians[i],iflow[i],"lapack");
    }
    return true;
}


//Voltages indexed by node ids
void SolvArmadillo::getVoltages(std::vector< std::vector<id_val> >& each,
                                std::vector<id_val>& all) {
    all = std::vector<id_val>(nbNodes());
    each = std::vector< std::vector<id_val> >(focals.size(),
                               std::vector<id_val>(nbNodes()));

    for (uint i = 0; i < focals.size(); i++) {
        int s = focals[i].first;
        int t = focals[i].second;
        if (s == t)
            continue;
        vec& vi = voltages[0];
        if (m == MULTI)
            vi = voltages[i];
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
            all[j].val += vi[idx];
            each[i][j].val += vi[idx];
        }
        
    }
    
}



#endif /*SOLVER_USE_ARMADILLO */
