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
    p.push_back(std::make_pair(89,1030));
    //p.push_back(std::make_pair(2,3));

    
    //SolvSoplex ss(p,SolvSoplex::UNIQUE);
    //SolvArmadillo ss(p,SolvSoplex::UNIQUE);
    SolvItPETSc ss(p,&argc,&argv,Solver::MULTI);
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
    sol.print(std::cout,1);

    std::vector<id_val> vs;
    std::vector<std::vector<id_val> > vss;
    ss.getVoltages(vss,vs);
    /*
    for (uint i = 0; i < vss.size(); i++)
        for (uint j = 0; j < vss[i].size(); j++)
            std::cout<<"v("<<vss[i][j].id<<") = "<<vss[i][j].val<<std::endl;
    //*/

    ls.findInitialSolution();
    sol = ls.getInitialSolution();
    sol.print(std::cout,1);

    ls.setTimeLimit(8);
    ls.solve();
    
    return 0;

    std::vector<id_val> upe;
    upe.push_back(id_val(2,20));
    ss.updateConductances(upe);

    ls.findBaseSolution();
    sol = ls.getBaseSolution();
    std::cout<<"Solved model. R = "<<sol.objective_<<std::endl;

    ss.getVoltages(vss,vs);


    for (uint i = 0; i < vss.size(); i++)
        for (uint j = 0; j < vss[i].size(); j++)
            std::cout<<"v("<<vss[i][j].id<<") = "<<vss[i][j].val<<std::endl;

    std::vector<id_val> x;
    std::vector<id_val> y;
    ss.getCurrents(x,y);

    for (uint i = 0; i < x.size(); i++)
        std::cout<<"c_n("<<x[i].id<<") = "<<x[i].val<<std::endl;

    
    
}
