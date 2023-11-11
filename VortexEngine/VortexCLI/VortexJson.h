#pragma once

#include <string>
#include <vector>
#include <map>

class JsonValue {
public:
    virtual ~JsonValue() {}
};

class JsonNumber : public JsonValue {
public:
    JsonNumber(double value);
    double getValue() const;

private:
    double value;
};

class JsonString : public JsonValue {
public:
    JsonString(const std::string& value);
    const std::string& getValue() const;

private:
    std::string value;
};

class JsonObject : public JsonValue {
public:
    void addProperty(const std::string& key, JsonValue* value);
    const std::map<std::string, JsonValue*>& getProperties() const;

private:
    std::map<std::string, JsonValue*> properties;
};

class JsonArray : public JsonValue {
public:
    void addElement(JsonValue* value);
    const std::vector<JsonValue*>& getElements() const;

private:
    std::vector<JsonValue*> elements;
};

class JsonParser {
public:
    JsonValue* parse(const std::string& json);

private:
    JsonValue* parseValue(const std::string& json, size_t& index);
    JsonObject* parseObject(const std::string& json, size_t& index);
    JsonArray* parseArray(const std::string& json, size_t& index);
    JsonString* parseString(const std::string& json, size_t& index);
    JsonNumber* parseNumber(const std::string& json, size_t& index);
};

class JsonPrinter {
public:
    void print(const JsonValue* jsonValue);

private:
    void printJsonObject(const JsonObject* jsonObject);
    void printJsonArray(const JsonArray* jsonArray);
    void printJsonString(const JsonString* jsonString);
    void printJsonNumber(const JsonNumber* jsonNumber);
};

