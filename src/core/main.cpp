#include <iostream>
#include <vector>
#include <time.h>
#include <stdlib.h>

#include "utils.hpp"
#include "electrical_circuit.hpp"
#include "solver_soplex.hpp"
#include "solver_armadillo.hpp"
#include "solver_iter_petsc.hpp"
#include "local_search_engine.hpp"
#include "pricing_manager.hpp"
#include "accepter.hpp"

int main(int argc, char* argv[]) {
    
    std::cout<< "=== Kea Landscape Tool ==="<<std::endl;
    srand(time(NULL));

    std::vector<std::pair<ElectricalCircuit::NodeID,ElectricalCircuit::NodeID> > p;
    //p.push_back(std::make_pair(89,1030));
    p.push_back(std::make_pair(0,10));
    //p.push_back(std::make_pair(4,15));
    
    //SolverSoplex ss(p,SolvSoplex::UNIQUE);
    //SolverArmadillo ss(p,SolvSoplex::UNIQUE);
    SolverPetsc ss(p,&argc,&argv,Solver::kOneMatrixAllPairs);
    ss.parseTextListFile(std::string(argv[1]));
    std::cout<<"Loaded file"<<std::endl;
    //ss.printECircuit();
    PricingManager pm(20);
    SimulatedAnnealing accepter;
    accepter.setTemperature(5);
    accepter.setCoolingRate(0.98);
    LocalSearchEngine ls(p,&ss,&pm,&accepter);
    std::cout<<"Created model"<<std::endl;


    JsonObject solutions;

    ls.findBaseSolution();
    Solution base_sol = ls.getBaseSolution();
    JsonObject* base_sol_json = base_sol.toJson();
    solutions.add("base_solution",base_sol_json);


    ls.findInitialSolution();
    Solution initial_sol = ls.getInitialSolution();
    JsonObject* initial_sol_json = initial_sol.toJson();
    solutions.add("initial_solution",initial_sol_json);

    ls.setTimeLimit(8);
    Solution best_sol = ls.solve(&solutions);
    JsonObject* best_sol_json = best_sol.toJson();
    solutions.add("best_solution",best_sol_json);

    std::cout<<solutions.toString()<<std::endl;
    


    return 0;

    
}
