#include "solver_wrapper.hpp"
#include <math.h>


Solver::Solver(const std::vector<std::pair<NodeID,NodeID> >& p, MultifocalMatrixMode m) :
    focals_(p), mode_(m) {

    for (auto pair : focals_) {
        focal_nodes_.insert(pair.first);
        focal_nodes_.insert(pair.second);
    }

}

bool Solver::compile() {
    for (uint i = 0; i < focals_.size(); i++)
        if (focals_[i].first < 0 || focals_[i].first >= nbNodes() ||
            focals_[i].second < 0 || focals_[i].second >= nbNodes())
            throw std::runtime_error("Invalid values for focal "
                                     +std::to_string(i)+"\n");
    if (nbNodes() <= 1)
        throw std::runtime_error("Invalid network: too few nodes\n");
    if (focals_.size() == 0)
        throw std::runtime_error("No focal specicified\n");
    return true;
}

void Solver::getCurrents(std::vector<id_val>& c_n, std::vector<id_val>& c_e) {

    std::vector<id_val> dummy;
    std::vector< std::vector<id_val> > vs;
    getVoltages(vs, dummy);
    c_n = std::vector<id_val>(nbNodes(),0);
    c_e = std::vector<id_val>(nbEdges(),0);

    for (int c = 0; c < vs.size(); c++) {
        for (int e = 0; e < nbEdges(); e++) {
            std::pair<NodeID,NodeID> uv = getNodes(e);
            int u = uv.first;
            int v = uv.second;
            c_e[e].id = e;
            c_e[e].val = fabs(vs[c][v].val - vs[c][u].val) * getConductance(e);
            c_n[u].id = u;
            if (isFocal(u)) {
                c_n[u].val += c_e[e].val;
            } else {
                c_n[u].val += c_e[e].val/2.0;
            }
            c_n[v].id = v;
            if (isFocal(v)) {
                c_n[v].val += c_e[e].val;
            } else {
                c_n[v].val += c_e[e].val/2.0;
            }

        }
    }

}
