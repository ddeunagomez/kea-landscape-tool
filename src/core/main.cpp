#include <iostream>
#include <vector>

#include "utils.hpp"
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

    std::vector<id_val> vs;
    ss.getVoltages(vs);

    for (uint i = 0; i < vs.size(); i++)
        std::cout<<"v("<<vs[i].id<<") = "<<vs[i].val<<std::endl;

    std::vector<uint> upe;
    upe.push_back(2);
    std::vector<double> upv;
    upv.push_back(20);
    ss.updateConductances(upe,upv);

    ss.solve();
    std::cout<<"Solved model"<<std::endl;

    ss.getVoltages(vs);

    for (uint i = 0; i < vs.size(); i++)
        std::cout<<"v("<<vs[i].id<<") = "<<vs[i].val<<std::endl;

    std::vector<id_val> x;
    std::vector<id_val> y;
    ss.getCurrents(x,y);

    for (uint i = 0; i < x.size(); i++)
        std::cout<<"c_n("<<x[i].id<<") = "<<x[i].val<<std::endl;

    std::sort(y.begin(), y.end(), id_val::sort_by_val);
    for (uint i = 0; i < y.size(); i++)
        std::cout<<"c_e("<<y[i].id<<") = "<<y[i].val<<std::endl;
    
}
