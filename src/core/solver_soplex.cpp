#ifdef SOLVER_USE_SOPLEX

#include "solver_soplex.hpp"
#include "utils.hpp"

using namespace soplex;

#define DEBUG 1

SolvSoplex::SolvSoplex(std::vector<std::pair<int,int> >& p,
                       Mode m) :
    Solver(p), m(m) {
    
}

SolvSoplex::~SolvSoplex() {

};

bool SolvSoplex::compile() {
    if(!Solver::compile()) return false;
    std::vector<std::pair<int,int> >& p = focals;
    assert(p.size());
    solvers.clear();
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
        int cshift = m == UNIQUE ? i*nbNodes() : sp.numColsReal();

        //std::cout<<"Num rows: "<<rshift<<std::endl;
        //std::cout<<"Num cols: "<<cshift<<std::endl;
        
        for (int j = 0; j < nbNodes(); j++) {
            DSVector row;
            row.add(nbNodes()*(m==UNIQUE ? p.size() : 1) - 1,0);
            //std::cout<<"Adding row of size "<< (nbNodes()-1)*(m==UNIQUE ? p.size() : 1)<<std::endl;
            sp.addRowReal(LPRow(row, LPRowReal::EQUAL, j == s ? 1.0 : 0.0));
        }
        
        for (int e = 0; e < nbEdges(); e++) {
            int u = getU(e);
            int v = getV(e);
            if (u == t || v == t) {
                continue;
            }
            sp.changeElementReal(rshift+u, cshift+v,
                                 sp.coefReal(rshift+u, cshift+v)-getCond(e));
            sp.changeElementReal(rshift+v, cshift+u,
                                 sp.coefReal(rshift+v, cshift+u)-getCond(e));
        }
        for (int n = 0; n < nbNodes(); n++) {
            if (n == t) continue;
            double s = 0;
            for (int j = 0; j < nbEdges(n); j++) {
                ECircuit::EdgeID e = getEdgeFrom(n,j);
                s += getCond(e);
            }
            sp.changeElementReal(rshift+n, cshift+n,s);
        }
        //std::cout<<"Num rows: "<<sp.numRowsReal()<<std::endl;
        //std::cout<<"Num cols: "<<sp.numColsReal()<<std::endl;
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
    return true;
}

bool SolvSoplex::updateConductances(std::vector<ECircuit::EdgeID> e,
                                     std::vector<double> v) {
    UNSUPPORTED;
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

bool SolvSoplex::getVoltages(std::vector<double>& sol) {

    sol.clear();
    sol = std::vector<double>(nbNodes(),0);
    if (m == MULTI) {
        for (uint i = 0; i < solvers.size(); i++) {
            SoPlex& s = solvers[i];
            DVector primal(s.numColsReal());
            if(!s.getPrimalReal(primal) ) {
                std::cerr<<"Error obtaining primal solution for solver "
                         <<i<<"."<<std::endl;
                return false;
            }
            assert(nbNodes() == s.numColsReal());
            for (int j = 0; j < nbNodes(); j++) {
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
            sol[j%nbNodes()] += primal[j];
        }
        
    }
    
    return true;
}

bool SolvSoplex::getCurrents(std::vector<double>& c_n,
                             std::vector<double>& c_e) {
    UNSUPPORTED;
    return true;
}

#endif /*SOLVER_USE_SOPLEX */
