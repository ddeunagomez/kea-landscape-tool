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
    p.push_back(std::make_pair(2,3));
    p.push_back(std::make_pair(0,4));
    SolvSoplex ss(g,p,SolvSoplex::UNIQUE);
    
}
