#include <iostream>
#include <vector>
#include <time.h>
#include <stdlib.h>

#include "utils.hpp"
#include "ecircuit.hpp"
#include "solver_soplex.hpp"
#include "solver_armadillo.hpp"
#include "solver_iter_petsc.hpp"
#include "local_search_engine.hpp"
#include "pricing_manager.hpp"
#include "accepter.hpp"

int main(int argc, char* argv[]) {
    
    std::cout<< "=== Kea Landscape Tool ==="<<std::endl;
    srand(time(NULL));

    std::vector<std::pair<int,int> > p;
    //p.push_back(std::make_pair(89,1030));
    p.push_back(std::make_pair(0,10));
    p.push_back(std::make_pair(4,15));
    
    //SolverSoplex ss(p,SolvSoplex::UNIQUE);
    //SolverArmadillo ss(p,SolvSoplex::UNIQUE);
    SolverPetsc ss(p,&argc,&argv,Solver::kOneMatrixPerPair);
    ss.parseTextListFile(std::string(argv[1]));
    std::cout<<"Loaded file"<<std::endl;
    //ss.printECircuit();
    PricingManager pm(20);
    SimulatedAnnealing accepter;
    accepter.setTemperature(5);
    accepter.setCoolingRate(0.98);
    LocalSearchEngine ls(p,&ss,&pm,&accepter);
    std::cout<<"Created model"<<std::endl;
    ls.findBaseSolution();
    Solution sol;
    sol = ls.getBaseSolution();
    sol.print(std::cout,10);
    std::cout<<sol.json()<<std::endl;


    ls.findInitialSolution();
    sol = ls.getInitialSolution();
    sol.print(std::cout,1);

    ls.setTimeLimit(8);
    ls.solve();
    
    return 0;

    
}
