#ifndef JSON_HPP
#define JSON_HPP

#include <string>
#include <vector>
#include <stdexcept>

/*
 *  This is a set of simple JSON classes for the purpose of this project.
 *  It is not intended to be a full JSON library, in any way!
 */


class JsonObject {
    std::vector<std::pair<std::string, JsonObject*>> map_;
public:

    /* Add pair (key,value)
     */
    virtual void add(std::string key, JsonObject* value);

    /* Serialize JSON object
     */
    virtual std::string toString() const;

    virtual ~JsonObject();
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
    JsonBool(bool v);
    std::string toString() const;
};

class JsonInt : public JsonLiteral {
    int value_;
public:
    JsonInt(int v);
    std::string toString() const;
};

class JsonFloat : public JsonLiteral {
    float value_;
    int precision_;
public:
    JsonFloat(float v, int precision = 5);
    std::string toString() const;
};

class JsonList : public JsonObject {
    std::vector<JsonObject*> elements_;
public:
    void add(JsonObject* o);
    void add(std::string key, JsonObject* value);
    std::string toString() const;
    virtual ~JsonList();
};

#endif // JSON_HPP
