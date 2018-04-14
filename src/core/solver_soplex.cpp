#ifdef SOLVER_USE_SOPLEX

#include "solver_soplex.hpp"
#include "utils.hpp"

using namespace soplex;

#define DEBUG 1

SolvSoplex::SolvSoplex(std::vector<std::pair<int,int> >& p,
                       Mode _m) :
    Solver(p), mode_(_m) {
}

SolvSoplex::~SolvSoplex() {

};

bool SolvSoplex::compile() {
    if(!Solver::compile()) return false;
    std::vector<std::pair<int,int> >& p = focals;
    assert(p.size());
    soplex_solvers_.clear();
    SoPlex s;
    soplex_solvers_.push_back(SoPlex());
    for (uint i = 0; i < p.size(); i++) {
        if (i > 0 && mode_ == MULTI)
            soplex_solvers_.push_back(SoPlex());

        SoPlex& sp = soplex_solvers_.back();
        sp.setIntParam(SoPlex::VERBOSITY, SoPlex::VERBOSITY_ERROR);
        int s = p[i].first;
        int t = p[i].second;

        int rshift = sp.numRowsReal();
        int cshift = mode_ == UNIQUE ? i*nbNodes() : sp.numColsReal();

        //std::cout<<"Num rows: "<<rshift<<std::endl;
        //std::cout<<"Num cols: "<<cshift<<std::endl;
        
        for (int j = 0; j < nbNodes(); j++) {
            DSVector row;
            row.add(nbNodes()*(mode_==UNIQUE ? p.size() : 1) - 1,0);
            //std::cout<<"Adding row of size "<< (nbNodes()-1)*(m==UNIQUE ? p.size() : 1)<<std::endl;
            sp.addRowReal(LPRow(row, LPRowReal::EQUAL, j == s ? 1.0 : 0.0));
        }
        
        for (int e = 0; e < nbEdges(); e++) {
            std::pair<NodeID,NodeID> uv = getNodes(e);
            int u = uv.first;
            int v = uv.second;
            if (u == t || v == t) {
                continue;
            }
            sp.changeElementReal(rshift+u, cshift+v,
                                 sp.coefReal(rshift+u, cshift+v)-getConductance(e));
            sp.changeElementReal(rshift+v, cshift+u,
                                 sp.coefReal(rshift+v, cshift+u)-getConductance(e));
        }
        for (int n = 0; n < nbNodes(); n++) {
            if (n == t) continue;
            double s = 0;
            for (int j = 0; j < nbEdges(n); j++) {
                ECircuit::EdgeID e = getEdgeFrom(n,j);
                s += getConductance(e);
            }
            sp.changeElementReal(rshift+n, cshift+n,s);
        }
        //std::cout<<"Num rows: "<<sp.numRowsReal()<<std::endl;
        //std::cout<<"Num cols: "<<sp.numColsReal()<<std::endl;
        sp.changeObjReal(cshift+s,1);
       
    }

    if (DEBUG) {
        if (mode_ == UNIQUE)
            soplex_solvers_[0].writeFileReal("initlpU.lp", NULL, NULL, NULL);
        else
            for (uint i = 0; i < soplex_solvers_.size(); i++) {
                std::string fn = "initlp"+std::to_string(i)+".lp";
                soplex_solvers_[i].writeFileReal(fn.c_str(), NULL, NULL, NULL);
            }
    }
    return true;
}


bool SolvSoplex::solve() {


    for (uint i = 0; i < soplex_solvers_.size(); i++) {
        SPxSolver::Status s;
        s = soplex_solvers_[i].solve();
        if (s != SPxSolver::OPTIMAL)
            return false;
    }
    
    return true;
}

void SolvSoplex::getVoltages(std::vector< std::vector<id_val> >& each,
                             std::vector<id_val>& all) {

    all = std::vector<id_val>(nbNodes());
    each = std::vector< std::vector<id_val> >(focals.size(),
                               std::vector<id_val>(nbNodes()));
    if (mode_ == MULTI) {
        for (uint i = 0; i < soplex_solvers_.size(); i++) {
            SoPlex& s = soplex_solvers_[i];
            DVector primal(s.numColsReal());
            if(!s.getPrimalReal(primal) ) {
                throw std::runtime_error("SoPlex: Error obtaining primal "
                                         "solution for solver "
                                         +std::to_string(i));
            }
            assert(nbNodes() == s.numColsReal());
            for (int j = 0; j < nbNodes(); j++) {
                all[j].id = j;
                all[j].val += primal[j];
                each[i][j].id = j;
                each[i][j].val += primal[j];
            }

        }
    } else {
        SoPlex& s = soplex_solvers_[0];
        DVector primal(s.numColsReal());
        if(!s.getPrimalReal(primal) ) {
            throw std::runtime_error("SoPlex: Error obtaining primal solution.");
        }
        for (int j = 0; j < s.numColsReal(); j++) {
            all[j%nbNodes()].id = j%nbNodes();
            all[j%nbNodes()].val += primal[j];
            each[j/nbNodes()][j].id = j;
            each[j/nbNodes()][j].val += primal[j];
        }
        
    }
    
}

#endif /*SOLVER_USE_SOPLEX */
