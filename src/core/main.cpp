#include <iostream>
#include <vector>

#include "ecircuit.hpp"
#include "solver_soplex.hpp"

int main(int argc, char* argv[]) {

    std::cout<< "=== Kea Landscape Tool ==="<<std::endl;


    ECircuit g;
    g.parseTextListFile(std::string(argv[1]));

    g.printECircuit();
    std::vector<std::pair<int,int> > p;
    //p.push_back(std::make_pair(2,3));
    p.push_back(std::make_pair(11,88));
    SolvSoplex ss(g,p,SolvSoplex::UNIQUE);
    ss.solve();

    std::vector<double> vs;
    ss.getCurrents(vs);

    for (uint i = 0; i < vs.size(); i++)
        std::cout<<"v("<<i<<") = "<<vs[i]<<std::endl;
    
}
