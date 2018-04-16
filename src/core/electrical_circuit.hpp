#ifndef _ECIRCUIT_HPP_
#define _ECIRCUIT_HPP_

#include <cassert>
#include <vector>
#include <string>

class ElectricalCircuit {
public:
    typedef unsigned int EdgeID;
    typedef unsigned int NodeID;
    typedef std::pair<NodeID,NodeID> Edge;

private:
    std::vector<Edge> edges_;
    std::vector<double> conductances_;
    std::vector< std::vector<EdgeID> > node_edge_map_;
    
public:
    
    ElectricalCircuit();
    ~ElectricalCircuit();

    //Returns the id of the new edge between u and v
    EdgeID addEdge(NodeID u, NodeID v, double cond = 0.0);

    //Returns the id of the new node
    NodeID addNode();

    inline int nbNodes() const {
        return node_edge_map_.size();
    }

    inline int nbEdges(int node = -1) const {
        assert(node >= -1);
        if (node == -1) {
            return edges_.size();
        } else {
            assert((uint)node < node_edge_map_.size());
            return node_edge_map_[node].size();
        }
    }

    inline EdgeID getEdgeFrom(NodeID n, uint e) {
        assert(n < node_edge_map_.size());
        assert(e < node_edge_map_[n].size());
        return node_edge_map_[n][e];
    }
    
    inline double getConductance(EdgeID e) {
        assert(e < conductances_.size());
        return conductances_[e];
    }

    inline void updateConductance(EdgeID e, double c) {
        assert(e < conductances_.size());
        conductances_[e] = c;
    }
    
    inline std::pair<NodeID,NodeID> getNodes(EdgeID e) {
        return edges_[e];
    }

    //Parse Circuitscape TextList file format
    bool parseTextListFile(std::string fname);
    
    void printECircuit();

};


#endif /* _ECIRCUIT_HPP_*/
