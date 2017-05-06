#include <iostream>
#include <fstream>
#include <sstream>
#include "ecircuit.hpp"
#include "utils.hpp"

using namespace std;

ECircuit::ECircuit() {

}

ECircuit::~ECircuit() {

}

ECircuit::EdgeID ECircuit::addEdge(NodeID u, NodeID v, double cond) {
    assert(u < node_edges.size());
    assert(v < node_edges.size());

    int e = edges.size();
    node_edges[u].push_back(e);
    node_edges[v].push_back(e);
    edges.push_back(std::make_pair(u,v));

    conds_old.push_back(cond);
    conds.push_back(cond);
    
    return e;
}

ECircuit::NodeID ECircuit::addNode() {
    int n = node_edges.size();
    node_edges.push_back(vector<EdgeID>());
    return n;
}


bool ECircuit::parseTextListFile(std::string fname) {
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
    this->printECircuit();
    infile.clear();
    infile.seekg(0,ios_base::beg);
    while (std::getline(infile, line))
    {
        std::istringstream iss(line);
        int a, b;
        double c;
        iss >> a >> b >> c;
        this->addEdge(a,b,c);
    }

    return true;
    
}

void ECircuit::printECircuit() {
    cout<<"{|"<<endl;
    cout<<"Nodes: "<< node_edges.size()<<endl;
    cout<<"Edges: "<< edges.size()<<endl;
    for (uint i = 0; i < node_edges.size(); i++) {
        cout<<"["<<i<<"]: ";
        for (uint j = 0; j < node_edges[i].size(); j++) {
            EdgeID e = node_edges[i][j];
            NodeID other = edges[e].first == i ?
                edges[e].second :
                edges[e].first;
            cout<<"("<<node_edges[i][j]<<","<<other<<","<<conds[e]<<") ";
        }
        cout<<endl;
    }
    cout<<"|}"<<endl;
}
