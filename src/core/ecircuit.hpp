#ifndef _ECIRCUIT_HPP_
#define _ECIRCUIT_HPP_

#include <cassert>
#include <vector>
#include <string>

class ECircuit {
public:
    typedef unsigned int EdgeID;
    typedef unsigned int NodeID;
    typedef std::pair<NodeID,NodeID> Edge;

private:
    std::vector<Edge> edges;
    std::vector<double> conds_old;
    std::vector<double> conds;
    std::vector< std::vector<EdgeID> > node_edges;
    
public:
    
    ECircuit();
    ~ECircuit();

    //Returns the id of the new edge between u and v
    EdgeID addEdge(NodeID u, NodeID v, double cond = 0.0);

    //Returns the id of the new node
    NodeID addNode();

    inline int nbNodes() const {return node_edges.size(); }
    inline int nbEdges(int node = -1) const {
        assert(node >= -1);
        if (node == -1) {
            return edges.size();
        } else {
            assert((uint)node < node_edges.size());
            return node_edges[node].size();
        }
    }

    inline EdgeID getEdgeFrom(NodeID n, uint e) {
        assert(n < node_edges.size());
        assert(e < node_edges[n].size());
        return node_edges[n][e];
    }
    
    inline double getCond(EdgeID e) {
        assert(e < conds.size());
        return conds[e];
    }
    inline double getOldCond(EdgeID e) {
        assert(e < conds.size());
        return conds_old[e];
    }
    inline void updateCond(EdgeID e, double c) {
        assert(e < conds.size());
        conds_old[e] = conds[e];
        conds[e] = c;
    }


    NodeID getU(EdgeID e) const {
        assert(e < edges.size());
        return edges[e].first;
    }

    NodeID getV(EdgeID e) const {
        assert(e < edges.size());
        return edges[e].second;
    }
    
    //Parse Circuitscape TextList file format
    bool parseTextListFile(std::string fname);
    
    void printECircuit();

};


#endif /* _ECIRCUIT_HPP_*/
