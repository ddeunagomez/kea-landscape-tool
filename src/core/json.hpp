#ifndef JSON_HPP
#define JSON_HPP

#include <string>
#include <vector>

/*
 *  This is a set of simple JSON classes for the purpose of this project.
 *  It is not intended to be a full JSON library, in any way!
 */


class JsonObject {
    std::vector<std::pair<std::string, JsonObject*>> map_;
public:
    virtual void add(std::string key, JsonObject* value) {
        if (value == this) {
            throw std::invalid_argument("Cannot aattach Json Object to itself.");
        }
        map_.push_back(std::make_pair(key,value));
    }
    virtual std::string toString() const {
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

    virtual ~JsonObject() {
        for (uint i = 0; i < map_.size(); i++)
            delete map_[i].second;
    }
};

class JsonLiteral : public JsonObject{
protected:
    JsonLiteral() { }
public:
    void add(std::string key, JsonObject* value) {
        throw std::invalid_argument("Cannot add to a JsonLiteral");
    }
};

class JsonBool : public JsonLiteral {
    bool value_;
public:
    JsonBool(bool v) : value_(v) { }
    std::string toString() const { return value_ ? "true" : "false"; }
};

class JsonInt : public JsonLiteral {
    int value_;
public:
    JsonInt(int v) : value_(v) { }
    std::string toString() const { return std::to_string(value_); }
};

class JsonFloat : public JsonLiteral {
    float value_;
    int precision_;
public:
    JsonFloat(float v, int precision = 5) : value_(v), precision_(precision) { }
    std::string toString() const { return std::to_string(value_); }
};

class JsonList : public JsonObject {
    std::vector<JsonObject*> elements_;
public:
    void add(JsonObject* o) {
        elements_.push_back(o);
    }
    void add(std::string key, JsonObject* value) {
        throw std::invalid_argument("Cannot add to a JsonList");
    }
    std::string toString() const {
        std::string result = "[";
        for (uint i = 0; i < elements_.size(); i++) {
            result += elements_[i]->toString() + (i == elements_.size() - 1 ? "" : ",");
        }
        result += "]";
        return result;
    }

    virtual ~JsonList() {
        for (uint i = 0; i < elements_.size(); i++)
            delete elements_[i];
    }
};

#endif // JSON_HPP
