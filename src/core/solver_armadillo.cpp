#ifdef SOLVER_USE_ARMADILLO

#include "solver_armadillo.hpp"
#include "utils.hpp"
#include <math.h>

using namespace arma;

SolvArmadillo::SolvArmadillo(std::vector<std::pair<int,int> >& p,
                             Mode m) : Solver(p), mode_(m) {

   
}

SolvArmadillo::~SolvArmadillo() {

}

bool SolvArmadillo::compile() {
    if(!Solver::compile()) return false;

    
    std::vector<std::pair<int,int> >& p = focals;
    laplacians_.clear();
    current_flow_.clear();
    voltages_.clear();

    int n = nbNodes();
    int dim = mode_ == MULTI ? n-1 : (n-1) * p.size();
    laplacians_.push_back(sp_mat(dim,dim));
    current_flow_.push_back(zeros<vec>(dim));
    voltages_.push_back(vec(dim));
    for (uint i = 0; i < p.size(); i++) {
        int s = p[i].first;
        int t = p[i].second;
        if (s == t)
            continue;
        if (i > 0 && mode_ == MULTI) {
            laplacians_.push_back(sp_mat(n - 1,n - 1));
            current_flow_.push_back(zeros<vec>(n - 1));
            voltages_.push_back(vec(n - 1));
        }
        sp_mat& lap = laplacians_.back();

        int rshift = mode_ == UNIQUE ? i*(n-1) : 0;
        int cshift = mode_ == UNIQUE ? i*(n-1) : 0;

        if (s < t)
            current_flow_.back()[rshift+s] = 1;
        else
            current_flow_.back()[rshift+s - 1] = 1;
        
        for (int e = 0; e < nbEdges(); e++) {
            std::pair<NodeID,NodeID> uv = getNodes(e);
            int u = uv.first;
            int v = uv.second;
            if (u == t || v == t) {
                continue;
            }            
            if (u > t) u--;
            if (v > t) v--;
            //std::cout<<"Access ("<<rshift+u<<","<<cshift+v<<")"<<std::endl;
            //std::cout<<"Access ("<<rshift+v<<","<<cshift+u<<")"<<std::endl;
            lap(rshift+u, cshift+v) = lap(rshift+u, cshift+v)-getConductance(e);
            lap(rshift+v, cshift+u) = lap(rshift+v, cshift+u)-getConductance(e);
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
                s += getConductance(e);
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
    for (uint i = 0; i < laplacians_.size(); i++) {
        voltages_[i] = spsolve(laplacians_[i],current_flow_[i],"lapack");
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
        vec& vi = voltages_[0];
        if (mode_ == MULTI)
            vi = voltages_[i];
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
            all[j].val += vi[idx];
            each[i][j].val += vi[idx];
        }
        
    }
    
}



#endif /*SOLVER_USE_ARMADILLO */
