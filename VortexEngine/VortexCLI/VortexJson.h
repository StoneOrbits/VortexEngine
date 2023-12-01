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
  JsonNumber(double value);
  double getValue() const;

private:
  double value;
};

class JsonString : public JsonValue
{
public:
  JsonString(const std::string &value);
  const std::string &getValue() const;

private:
  std::string value;
};

class JsonObject : public JsonValue
{
public:
  virtual ~JsonObject();
  void addProperty(const std::string &key, JsonValue *value);
  const std::map<std::string, JsonValue *> &getProperties() const;

private:
  std::map<std::string, JsonValue *> properties;
};

class JsonArray : public JsonValue
{
public:
  virtual ~JsonArray();
  void addElement(JsonValue *value);
  const std::vector<JsonValue *> &getElements() const;

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
