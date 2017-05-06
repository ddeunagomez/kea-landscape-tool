#include <iostream>

#include "ecircuit.hpp"


int main(int argc, char* argv[]) {

    std::cout<< "=== Kea Landscape Tool ==="<<std::endl;


    ECircuit g;
    g.parseTextListFile(std::string(argv[1]));

    g.printECircuit();
    
}
