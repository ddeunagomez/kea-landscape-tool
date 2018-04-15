#ifndef _SOLVER_WRAPPER_HPP_
#define _SOLVER_WRAPPER_HPP_

#include "electrical_circuit.hpp"
#include <vector>
#include "utils.hpp"

/*
 * Wrapper around whatever solver I am using. 
 * Using SoPlex at the moment, but might need to test others
 * If I do local search the solver doesn't actually need to be as
 * clever as soplex or cplex, just needs to solve a system of equations
 * Using soplex at the moment in case I want to test LNS instead of LS
 */
class Solver : public ElectricalCircuit {
public:
    enum MultifocalMatrixMode {kOneMatrixAllPairs, kOneMatrixPerPair};
protected:
    std::vector<std::pair<int,int> > focals_;
    MultifocalMatrixMode mode_;
public:
    //Electrical circuit and pairs of focals
    Solver(std::vector<std::pair<int,int> >& p, MultifocalMatrixMode m = MultifocalMatrixMode::kOneMatrixPerPair) :
        focals_(p), mode_(m) {}

    virtual ~Solver() {}

    virtual bool compile() {
        for (uint i = 0; i < focals_.size(); i++)
            if (focals_[i].first < 0 || focals_[i].first >= nbNodes() ||
                focals_[i].second < 0 || focals_[i].second >= nbNodes())
                throw std::runtime_error("Invalid values for focal "
                                         +std::to_string(i)+"\n");
        if (nbNodes() <= 1)
            throw std::runtime_error("Invalid network: too few nodes\n");
        if (focals_.size() == 0)
            throw std::runtime_error("No focal specicified\n");
        return true;
    }
    
    virtual void updateConductances(std::vector<id_val>& ev) {
        throw unimplemented_solver("updateConductances");
    }
    
    virtual bool solve() {
        throw unimplemented_solver("solve");
    };

    //Voltages indexed by node ids
    virtual void getVoltages(std::vector< std::vector<id_val> >& each,
                             std::vector<id_val>& all) {
        throw unimplemented_solver("getVoltages");
    };
    //Currents by node and by edge
    virtual void getCurrents(std::vector<id_val>& c_n,
                             std::vector<id_val>& c_e) {

        throw unimplemented_solver("getCurrents");
    }

};

#endif /*_SOLVER_WRAPPER_HPP_ */
