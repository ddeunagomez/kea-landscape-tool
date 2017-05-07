#include "solver_soplex.hpp"

using namespace soplex;

#define DEBUG 1

SolvSoplex::SolvSoplex(ECircuit& ec, std::vector<std::pair<int,int> >& p,
                       Mode m) :
    Solver(ec,p), m(m) {
    assert(p.size());
    SoPlex s;
    solvers.push_back(SoPlex());
    for (uint i = 0; i < p.size(); i++) {
        if (i > 0 && m == MULTI)
            solvers.push_back(SoPlex());

        SoPlex& sp = solvers.back();
        sp.setIntParam(SoPlex::VERBOSITY, SoPlex::VERBOSITY_ERROR);
        int s = p[i].first;
        int t = p[i].second;

        int rshift = sp.numRowsReal();
        int cshift = m == UNIQUE ? i*ec.nbNodes() : sp.numColsReal();

        std::cout<<"Num rows: "<<rshift<<std::endl;
        std::cout<<"Num cols: "<<cshift<<std::endl;
        
        for (int j = 0; j < ec.nbNodes(); j++) {
            DSVector row;
            row.add(ec.nbNodes()*(m==UNIQUE ? p.size() : 1) - 1,0);
            std::cout<<"Adding row of size "<< (ec.nbNodes()-1)*(m==UNIQUE ? p.size() : 1)<<std::endl;
            sp.addRowReal(LPRow(row, LPRowReal::EQUAL, j == s ? 1.0 : 0.0));
        }
        
        for (int e = 0; e < ec.nbEdges(); e++) {
            int u = ec.getU(e);
            int v = ec.getV(e);
            if (u == t || v == t) {
                continue;
            }
            sp.changeElementReal(rshift+u, cshift+v,
                                 sp.coefReal(rshift+u, cshift+v)-ec.getCond(e));
            sp.changeElementReal(rshift+v, cshift+u,
                                 sp.coefReal(rshift+v, cshift+u)-ec.getCond(e));
        }
        for (int n = 0; n < ec.nbNodes(); n++) {
            if (n == t) continue;
            double s = 0;
            for (int j = 0; j < ec.nbEdges(n); j++) {
                ECircuit::EdgeID e = ec.getEdgeFrom(n,j);
                s += ec.getCond(e);
            }
            sp.changeElementReal(rshift+n, cshift+n,s);
        }
        std::cout<<"Num rows: "<<sp.numRowsReal()<<std::endl;
        std::cout<<"Num cols: "<<sp.numColsReal()<<std::endl;
        sp.changeObjReal(cshift+s,1);
       
    }

    if (DEBUG) {
        if (m == UNIQUE)
            solvers[0].writeFileReal("initlpU.lp", NULL, NULL, NULL);
        else
            for (uint i = 0; i < solvers.size(); i++) {
                std::string fn = "initlp"+std::to_string(i)+".lp";
                solvers[i].writeFileReal(fn.c_str(), NULL, NULL, NULL);
            }
    }
    
}

SolvSoplex::~SolvSoplex() {

};

bool SolvSoplex::updateConductance(ECircuit::EdgeID e, double v) {


    return true;
}

bool SolvSoplex::solve() {


    for (uint i = 0; i < solvers.size(); i++) {
        SPxSolver::Status s;
        s = solvers[i].solve();
        if (s != SPxSolver::OPTIMAL)
            return false;
    }
    
    return true;
}

bool SolvSoplex::getCurrents(std::vector<double>& sol) {

    sol.clear();
    sol = std::vector<double>(ec.nbNodes(),0);
    if (m == MULTI) {
        for (uint i = 0; i < solvers.size(); i++) {
            SoPlex& s = solvers[i];
            DVector primal(s.numColsReal());
            if(!s.getPrimalReal(primal) ) {
                std::cerr<<"Error obtaining primal solution for solver "
                         <<i<<"."<<std::endl;
                return false;
            }
            assert(ec.nbNodes() == s.numColsReal());
            for (int j = 0; j < ec.nbNodes(); j++) {
                sol[j] += primal[j];
            }

        }
    } else {
        SoPlex& s = solvers[0];
        DVector primal(s.numColsReal());
        if(!s.getPrimalReal(primal) ) {
            std::cerr<<"Error obtaining primal solution."<<std::endl;
            return false;
        }
        for (int j = 0; j < s.numColsReal(); j++) {
            sol[j%ec.nbNodes()] += primal[j];
        }
        
    }
    
    return true;
}

