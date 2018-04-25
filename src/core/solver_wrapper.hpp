#ifndef _SOLVER_WRAPPER_HPP_
#define _SOLVER_WRAPPER_HPP_

#include "electrical_circuit.hpp"
#include <vector>
#include <unordered_set>
#include "utils.hpp"

/*
 * Wrapper around whatever solver I am using. 
 * Using SoPlex at the moment, but might need to test others
 * If I do local search the solver doesn't actually need to be as
 * clever as soplex or cplex, just needs to solve a system of equations
 * Using soplex at the moment in case I want to test LNS instead of LS
 */

enum AvailableSolvers {kSoplex, kArmadillo, kPetSc};

class Solver : public ElectricalCircuit {
public:
    enum MultifocalMatrixMode {kOneMatrixAllPairs, kOneMatrixPerPair};
protected:
    std::vector<std::pair<NodeID,NodeID> > focals_;
    std::unordered_set<NodeID> focal_nodes_;
    MultifocalMatrixMode mode_;

    inline bool isFocal(NodeID n) const {
        return focal_nodes_.find(n) != focal_nodes_.end();
    }

public:
    Solver(const std::vector<std::pair<NodeID,NodeID> >& p, MultifocalMatrixMode m = MultifocalMatrixMode::kOneMatrixPerPair);

    virtual ~Solver() {}

    /* Gets the backend solver ready.
     * Only to be called after feeding all the conductances/edges and nodes to the object.
     * It verifies that the focals given are valid (within the terrain).
     */
    virtual bool compile();
    

    /* Update the conductances of the electrical network.
     * Given by pairs (EdgeID, conductance (in ohm^-1))
     */
    virtual void updateConductances(std::vector<id_val>& ev) {
        throw unimplemented_solver("updateConductances");
    }
    
    /* Runs the backend solver.
     * Return true if the solver succeeds. False otherwise
     */
    virtual bool solve() {
        throw unimplemented_solver("solve");
    };

    /* Voltages are given in pairs (NodeID, voltage)
     * each: there will be one voltage for each node, for each circuit (in case of multiple pairs of focals).
     * all: there will be one voltage for each node, all circuits combined (summed up).
     */
    virtual void getVoltages(std::vector< std::vector<id_val> >& each,
                             std::vector<id_val>& all) {
        throw unimplemented_solver("getVoltages");
    };

    //Currents by node and by edge
    virtual void getCurrents(std::vector<id_val>& c_n,
                             std::vector<id_val>& c_e);

};

#endif /*_SOLVER_WRAPPER_HPP_ */
