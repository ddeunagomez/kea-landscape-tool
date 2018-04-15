#include "solution.hpp"

using namespace std;

void Solution::print(std::ostream& o, int level) {
    o <<"R = "<<std::fixed<<std::setprecision(10)<<objective_<<"\n";
    if (level < 1) return;
    int i = 0;
    o <<"Sol = [";
    for (auto it = chosen_alternative_.begin(); it != chosen_alternative_.end(); ++it) {
        if (it->second != 0) {
            o<<it->first<<",";
            i++;
        }
    }
    o<<"]"<<i<<"\n";
    if (level < 2) return;
    o <<"Vn = [";
    for (uint j = 0; j < voltages_.size(); j++) {
        for (uint i = 0; i < voltages_[j].size(); i++) {
            o<<"("<<voltages_[j][i].id<<","<<voltages_[j][i].val<<"),";
        }
        if (j < voltages_.size() - 1)
            o<<"\n      ";
    }
    o<<"]\n";
    if (level < 3) return;
    o <<"Cn = [";
    for (uint i = 0; i < node_currents_.size(); i++) {
        o<<"("<<node_currents_[i].id<<","<<node_currents_[i].val<<"),";
    }
    o<<"]\n";
    if (level < 4) return;
    o <<"Ce = [";
    for (uint i = 0; i < edge_currents_.size(); i++) {
        o<<"("<<edge_currents_[i].id<<","<<edge_currents_[i].val<<"),";
    }
    o<<"]\n";
}

string Solution::json() {
    string result = "{\n";

    result += "\"nodes\" : " + to_string(node_currents_.size()) +"\n";
    result += "\"edges\" : " + to_string(edge_currents_.size()) +"\n";

    result += "\"objective\" : " + to_string(objective_) +"\n";

    result += "\"investments\" : [";
    vector<int> invest;
    for (auto it = chosen_alternative_.begin(); it != chosen_alternative_.end(); ++it)
        if (it->second != 0)
            invest.push_back(it->first);
    sort(invest.begin(), invest.end());
    for (int i = 0; i < invest.size(); i++) {
        result += to_string(invest[i]) + (i == invest.size() - 1 ? "" :",");
    }
    result += "]\n";


    result += "\"current_nodes\" : [";
    sort(node_currents_.begin(),node_currents_.end(),id_val::sort_by_id);
    for (uint i = 0; i < node_currents_.size(); i++) {
        result += to_string(node_currents_[i].val) + (i == node_currents_.size() - 1 ? "" :",");;
    }
    result += "]\n";


    result += "\"current_edges\" : [";
    sort(edge_currents_.begin(),edge_currents_.end(),id_val::sort_by_id);
    for (uint i = 0; i < edge_currents_.size(); i++) {
        result += to_string(edge_currents_[i].val) + (i == edge_currents_.size() - 1 ? "" :",");;
    }
    result += "]\n";

    result += "}\n";
    return result;
}
