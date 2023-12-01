#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <map>

class JsonValue
{
public:
  virtual ~JsonValue() {}
};

class JsonNumber : public JsonValue
{
public:
  JsonNumber(double value) : value(value) {}
  double getValue() const { return value; }

private:
  double value;
};

class JsonString : public JsonValue
{
public:
  JsonString(const std::string &value) : value(value) {}
  const std::string &getValue() const { return value; }

private:
  std::string value;
};

class JsonObject : public JsonValue
{
public:
  virtual ~JsonObject() {
    for (auto& pair : properties) {
      delete pair.second;
    }
    properties.clear();
  }
  void addProperty(const std::string &key, JsonValue *value) { properties[key] = value; }
  const std::map<std::string, JsonValue *> &getProperties() const { return properties; }

private:
  std::map<std::string, JsonValue *> properties;
};

class JsonArray : public JsonValue
{
public:
  virtual ~JsonArray() {
    for (auto& el : elements) {
      delete el;
    }
    elements.clear();
  }
  void addElement(JsonValue *value) { elements.push_back(value); }
  const std::vector<JsonValue *> &getElements() const { return elements; }

private:
  std::vector<JsonValue *> elements;
};

class JsonParser
{
public:
  JsonValue *parseJson(const std::string &json);
  JsonValue *parseJsonFromFile(const std::string &filename);

private:
  JsonValue *parseFromStream(std::istream &stream);
  JsonValue *parseValue(const std::string &json, size_t &index);
  JsonObject *parseObject(const std::string &json, size_t &index);
  JsonArray *parseArray(const std::string &json, size_t &index);
  JsonString *parseString(const std::string &json, size_t &index);
  JsonNumber *parseNumber(const std::string &json, size_t &index);
};

class JsonPrinter
{
public:
  void printJson(const JsonValue *jsonValue, bool prettyPrint = false);
  void writeJson(std::ofstream &file, const JsonValue *jsonValue, bool prettyPrint = false);

private:
  void printJsonInternal(const JsonValue *jsonValue, bool prettyPrint, int indentation);
  void writeToStream(std::ostream &stream, const JsonValue *jsonValue, bool prettyPrint, int indentation);

  // New sub-functions
  void printJsonObject(std::ostream &stream, const JsonObject *jsonObject, bool prettyPrint, int indentation);
  void printJsonArray(std::ostream &stream, const JsonArray *jsonArray, bool prettyPrint, int indentation);
  void printJsonString(std::ostream &stream, const JsonString *jsonString);
  void printJsonNumber(std::ostream &stream, const JsonNumber *jsonNumber);
};
