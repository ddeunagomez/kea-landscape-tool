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
class Solver{
    ECircuit ec; //Internal copy, nobody can modify it.
    std::vector<std::pair<int,int> > focals;
public:
    //Pass number of variables
    Solver(ECircuit& ec, std::vector<std::pair<int,int> >& p) :
    ec(ec), focals(p) {};
    virtual ~Solver() {};

    virtual bool updateConductance(ECircuit::EdgeID e, double v) = 0;
    virtual bool solve() = 0;

    //Currents indexed by node ids
    virtual bool getCurrents(std::vector<double>& sol) = 0;
};

#endif /*_SOLVER_WRAPPER_HPP_ */
