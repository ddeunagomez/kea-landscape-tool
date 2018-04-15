#include <iostream>
#include <fstream>
#include <sstream>
#include "electrical_circuit.hpp"
#include "utils.hpp"

using namespace std;

ElectricalCircuit::ElectricalCircuit() {

}

ElectricalCircuit::~ElectricalCircuit() {

}

ElectricalCircuit::EdgeID ElectricalCircuit::addEdge(NodeID u, NodeID v, double cond) {
    assert(u < node_edge_map_.size());
    assert(v < node_edge_map_.size());

    int e = edges_.size();
    node_edge_map_[u].push_back(e);
    node_edge_map_[v].push_back(e);
    edges_.push_back(std::make_pair(u,v));

    conductances_.push_back(cond);
    
    return e;
}

ElectricalCircuit::NodeID ElectricalCircuit::addNode() {
    int n = node_edge_map_.size();
    node_edge_map_.push_back(vector<EdgeID>());
    return n;
}


bool ElectricalCircuit::parseTextListFile(std::string fname) {
    if (!file_exists(fname)) {
        cerr<<"File "<<fname<<" does not exist."<<endl;
        return false;
    }
    std::ifstream infile(fname);


    string line;
    int max_node = 0;
    while (std::getline(infile, line))
    {
        std::istringstream iss(line);
        int a, b;
        double c;
        if (!(iss >> a >> b >> c) || a < 0 || b < 0 || c < 0) {
            cerr<<"Your file "<<fname<<" is not correctly formatted" 
                " as a Text List File."<<endl<<"Each line should contain"
                " x y z , where x and y are integers and z is a floatting"
                " point number, all positive or zero."<<endl;
            return false;
        }

        max_node = MAX(max_node,a);
        max_node = MAX(max_node,b);
    }
    for (int i = 0; i < max_node + 1; i++)
        this->addNode();
    //this->printECircuit();
    infile.clear();
    infile.seekg(0,ios_base::beg);
    while (std::getline(infile, line))
    {
        std::istringstream iss(line);
        int a, b;
        double c;
        iss >> a >> b >> c;
        this->addEdge(a,b,1.0/c);
    }

    return true;
    
}

void ElectricalCircuit::printECircuit() {
    cout<<"{|"<<endl;
    cout<<"Nodes: "<< node_edge_map_.size()<<endl;
    cout<<"Edges: "<< edges_.size()<<endl;
    for (uint i = 0; i < node_edge_map_.size(); i++) {
        cout<<"["<<i<<"]: ";
        for (uint j = 0; j < node_edge_map_[i].size(); j++) {
            EdgeID e = node_edge_map_[i][j];
            NodeID other = edges_[e].first == i ?
                edges_[e].second :
                edges_[e].first;
            cout<<"("<<node_edge_map_[i][j]<<","<<other<<","<<conductances_[e]<<") ";
        }
        cout<<endl;
    }
    cout<<"|}"<<endl;
}
