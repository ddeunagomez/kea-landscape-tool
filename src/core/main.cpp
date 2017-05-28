#include <iostream>
#include <vector>

#include "utils.hpp"
#include "ecircuit.hpp"
#include "solver_soplex.hpp"
#include "solver_armadillo.hpp"
#include "solver_iter_petsc.hpp"
#include "local_search_engine.hpp"
#include "pricing_manager.hpp"

int main(int argc, char* argv[]) {

    std::cout<< "=== Kea Landscape Tool ==="<<std::endl;


    std::vector<std::pair<int,int> > p;
    p.push_back(std::make_pair(0,4));
    p.push_back(std::make_pair(2,3));

    
    //SolvSoplex ss(p,SolvSoplex::UNIQUE);
    //SolvArmadillo ss(p,SolvSoplex::UNIQUE);
    SolvItPETSc ss(p,&argc,&argv,Solver::UNIQUE);
    ss.parseTextListFile(std::string(argv[1]));
    std::cout<<"Loaded file"<<std::endl;
    ss.printECircuit();
    PricingManager pm(3);
    LocalSearchEngine ls(p,&ss,&pm);
    std::cout<<"Created model"<<std::endl;
    double bsol = ls.baseSolution();
    std::cout<<"Solved model. R = "<<bsol<<std::endl;

    std::vector<id_val> vs;
    std::vector<std::vector<id_val> > vss;
    ss.getVoltages(vss,vs);

    for (uint i = 0; i < vss.size(); i++)
        for (uint j = 0; j < vss[i].size(); j++)
            std::cout<<"v("<<vss[i][j].id<<") = "<<vss[i][j].val<<std::endl;

    std::vector<uint> upe;
    upe.push_back(2);
    std::vector<double> upv;
    upv.push_back(20);
    ss.updateConductances(upe,upv);

    bsol = ls.baseSolution();
    std::cout<<"Solved model. R = "<<bsol<<std::endl;

    ss.getVoltages(vss,vs);


    for (uint i = 0; i < vss.size(); i++)
        for (uint j = 0; j < vss[i].size(); j++)
            std::cout<<"v("<<vss[i][j].id<<") = "<<vss[i][j].val<<std::endl;

    std::vector<id_val> x;
    std::vector<id_val> y;
    ss.getCurrents(x,y);

    for (uint i = 0; i < x.size(); i++)
        std::cout<<"c_n("<<x[i].id<<") = "<<x[i].val<<std::endl;

    std::sort(y.begin(), y.end(), id_val::sort_by_val);
    for (uint i = 0; i < y.size(); i++)
        std::cout<<"c_e("<<y[i].id<<") = "<<y[i].val<<std::endl;
    
}
