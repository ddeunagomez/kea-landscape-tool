#include "solution.hpp"

using namespace std;

void Solution::print(std::ostream& o, int level) const {
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

JsonObject* Solution::toJson() const {
    JsonObject* jo = new JsonObject();

    jo->add("nodes",new JsonInt(node_currents_.size()));
    jo->add("edges",new JsonInt(edge_currents_.size()));
    jo->add("resistance", new JsonFloat(objective_));

    JsonList* investments = new JsonList();
    jo->add("investments",investments);
    vector<int> tmp;
    for (auto it = chosen_alternative_.begin(); it != chosen_alternative_.end(); ++it)
        if (it->second != 0)
            tmp.push_back(it->first);
    sort(tmp.begin(), tmp.end());
    for (uint i = 0; i < tmp.size(); i++) {
        investments->add(new JsonInt(tmp[i]));
    }

    JsonList* node_currs = new JsonList();
    jo->add("node_currents",node_currs);
    vector<id_val> currs = node_currents_;
    sort(currs.begin(),currs.end(),id_val::sort_by_id);
    for (uint i = 0; i < currs.size(); i++) {
        node_currs->add(new JsonFloat(currs[i].val));
    }

    return jo;
}
