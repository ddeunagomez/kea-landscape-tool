#include <iostream>

#include "graph.hpp"


int main(int argc, char* argv[]) {

    std::cout<< "=== Kea Landscape Tool ==="<<std::endl;


    Graph g;
    g.parseTextListFile(std::string(argv[1]));

    g.printGraph();
    
}
