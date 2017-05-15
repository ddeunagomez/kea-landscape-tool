#include <iostream>
#include <vector>

#include "ecircuit.hpp"
#include "solver_soplex.hpp"
#include "solver_armadillo.hpp"
#include "solver_iter_petsc.hpp"

int main(int argc, char* argv[]) {

    std::cout<< "=== Kea Landscape Tool ==="<<std::endl;


    ECircuit g;
    g.parseTextListFile(std::string(argv[1]));
    std::cout<<"Loaded file"<<std::endl;
    //g.printECircuit();
    std::vector<std::pair<int,int> > p;
    p.push_back(std::make_pair(12,88));
    //p.push_back(std::make_pair(101,888));
    //SolvSoplex ss(g,p,SolvSoplex::UNIQUE);
    SolvArmadillo ss(g,p,SolvSoplex::MULTI);
    //SolvItPETSc ss(g,p,&argc,&argv,SolvSoplex::UNIQUE);
    std::cout<<"Created model"<<std::endl;
    ss.solve();
    std::cout<<"Solved model"<<std::endl;

    std::vector<double> vs;
    ss.getVoltages(vs);

    for (uint i = 0; i < vs.size(); i++)
        std::cout<<"v("<<i<<") = "<<vs[i]<<std::endl;
    
}
