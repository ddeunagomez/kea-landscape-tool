#include <cassert>
#include <vector>

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

    //Parse Circuitscape TextList file format
    bool parseTextListFile(std::string fname);
    
    void printECircuit();
};
