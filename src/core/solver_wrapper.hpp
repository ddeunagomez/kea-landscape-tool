#ifndef _SOLVER_WRAPPER_HPP_
#define _SOLVER_WRAPPER_HPP_

#include "ecircuit.hpp"
#include <vector>
#include "utils.hpp"

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
                throw std::runtime_error("Invalid values for focal "
                                         +std::to_string(i)+"\n");
        if (nbNodes() <= 1)
            throw std::runtime_error("Invalid network: too few nodes\n");
        if (focals.size() == 0)
            throw std::runtime_error("No focal specicified\n");
        return true;
    }
    
    virtual void updateConductances(std::vector<ECircuit::EdgeID> e,
                                    std::vector<double> v) {
        throw unimplemented_solver("updateConductances");
    }
    
    virtual bool solve() {
        throw unimplemented_solver("solve");
    };

    //Voltages indexed by node ids
    virtual void getVoltages(std::vector<id_val>& sol) {
        throw unimplemented_solver("getVoltages");
    };
    //Currents by node and by edge
    virtual void getCurrents(std::vector<id_val>& c_n,
                             std::vector<id_val>& c_e) {

        throw unimplemented_solver("getCurrents");
    }

};

#endif /*_SOLVER_WRAPPER_HPP_ */
