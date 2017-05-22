#ifndef _SOLVER_WRAPPER_HPP_
#define _SOLVER_WRAPPER_HPP_

#include "ecircuit.hpp"
#include <vector>

/*
 * Wrapper around whatever solver I am using. 
 * Using SoPlex at the moment, but might need to test others
 * If I do local search the solver doesn't actually need to be as
 * clever as soplex or cplex, just needs to solve a system of equations
 * Using soplex at the moment in case I want to test LNS instead of LS
 */
class Solver : public ECircuit {

protected:
    std::vector<std::pair<int,int> > focals;
public:
    enum Mode {UNIQUE, MULTI};
    //Electrical circuit and pairs of focals
    Solver(std::vector<std::pair<int,int> >& p) :
        focals(p) {};    
    virtual ~Solver() {};

    virtual bool compile() {
        for (uint i = 0; i < focals.size(); i++)
            if (focals[i].first < 0 || focals[i].first >= nbNodes() ||
                focals[i].second < 0 || focals[i].second >= nbNodes())
                return false;
        return nbNodes() > 1 && focals.size() > 0;
    }
    
    virtual bool updateConductances(std::vector<ECircuit::EdgeID> e,
                                    std::vector<double> v) = 0;
    virtual bool solve() = 0;

    //Voltages indexed by node ids
    virtual bool getVoltages(std::vector<double>& sol) = 0;

};

#endif /*_SOLVER_WRAPPER_HPP_ */
