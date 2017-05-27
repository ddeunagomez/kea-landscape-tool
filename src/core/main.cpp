#include <iostream>
#include <vector>

#include "ecircuit.hpp"
#include "solver_soplex.hpp"
#include "solver_armadillo.hpp"
#include "solver_iter_petsc.hpp"

int main(int argc, char* argv[]) {

    std::cout<< "=== Kea Landscape Tool ==="<<std::endl;


    std::vector<std::pair<int,int> > p;
    p.push_back(std::make_pair(0,4));
    //p.push_back(std::make_pair(101,888));

    
    //SolvSoplex ss(p,SolvSoplex::UNIQUE);
    //SolvArmadillo ss(p,SolvSoplex::UNIQUE);
    SolvItPETSc ss(p,&argc,&argv,SolvSoplex::UNIQUE);
    ss.parseTextListFile(std::string(argv[1]));
    std::cout<<"Loaded file"<<std::endl;
    ss.printECircuit();
    ss.compile();
    std::cout<<"Created model"<<std::endl;
    ss.solve();
    std::cout<<"Solved model"<<std::endl;

    std::vector<double> vs;
    ss.getVoltages(vs);

    for (uint i = 0; i < vs.size(); i++)
        std::cout<<"v("<<i<<") = "<<vs[i]<<std::endl;

    std::vector<uint> upe;
    upe.push_back(2);
    std::vector<double> upv;
    upv.push_back(20);
    ss.updateConductances(upe,upv);

    ss.solve();
    std::cout<<"Solved model"<<std::endl;

    ss.getVoltages(vs);

    for (uint i = 0; i < vs.size(); i++)
        std::cout<<"v("<<i<<") = "<<vs[i]<<std::endl;
    
}
