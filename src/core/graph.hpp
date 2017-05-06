#include <vector>

class Graph {
public:
    typedef int EdgeID;
    typedef std::pair<int,int> Edge;
    typedef int NodeID;
private:
    vector<Edge> edges;
    vector< vector<EdgeID> > node_edges;
    
public:

    Graph();
    ~Graph();

    //Returns the id of the new edge between u and v
    EdgeID addEdge(NodeID u, NodeID v);

    //Returns the id of the new node
    NodeID addNode();

    

};
