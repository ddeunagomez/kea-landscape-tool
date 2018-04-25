#include "json.hpp"
#include <stdexcept>

//JSON object: main class

void JsonObject::add(std::string key, JsonObject* value) {
    if (value == this) {
        throw std::invalid_argument("Cannot attach Json Object to itself.");
    }
    map_.push_back(std::make_pair(key,value));
}

std::string JsonObject::toString() const {
    std::string result = "{\n";
    for (uint i = 0; i < map_.size(); i++) {
        std::string k = map_[i].first;
        JsonObject* jo = map_[i].second;
        result += "\"" + k +"\" : ";
        result += jo->toString();
        if (i < map_.size() -1 )
            result +=",\n";
    }
    result += "\n}";
    return result;
}

JsonObject::~JsonObject() {
    for (uint i = 0; i < map_.size(); i++)
        delete map_[i].second;
}



//JSON Bool: for booleans

JsonBool::JsonBool(bool v) : value_(v) {

}

std::string JsonBool::toString() const {
    return value_ ? "true" : "false";
}

//JSON Int: for integers

JsonInt::JsonInt(int v) : value_(v) { }

std::string JsonInt::toString() const {
    return std::to_string(value_);
}

//JSON Float: for floats
JsonFloat::JsonFloat(float v, int precision) : value_(v), precision_(precision) {

}

std::string JsonFloat::toString() const {
    return std::to_string(value_);
}

//JSON List: contains list of JSON objects

void JsonList::add(JsonObject* o) {
    elements_.push_back(o);
}

void JsonList::add(std::string key, JsonObject* value) {
    throw std::invalid_argument("Cannot add to a JsonList");
}

std::string JsonList::toString() const {
    std::string result = "[";
    for (uint i = 0; i < elements_.size(); i++) {
        result += elements_[i]->toString() + (i == elements_.size() - 1 ? "" : ",");
    }
    result += "]";
    return result;
}

JsonList::~JsonList() {
    for (uint i = 0; i < elements_.size(); i++)
        delete elements_[i];
}

