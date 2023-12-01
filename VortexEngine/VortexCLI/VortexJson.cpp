#include "VortexJson.h"
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;

JsonNumber::JsonNumber(double value) : value(value) {}

double JsonNumber::getValue() const
{
  return value;
}

JsonString::JsonString(const string &value) : value(value) {}

const string &JsonString::getValue() const
{
  return value;
}

JsonObject::~JsonObject()
{
  // Cleanup properties
  for (auto& pair : properties) {
    delete pair.second; // Release memory occupied by JsonValue objects
  }
  properties.clear(); // Clear the map
}

void JsonObject::addProperty(const string &key, JsonValue *value)
{
  properties[key] = value;
}

const map<string, JsonValue *> &JsonObject::getProperties() const
{
  return properties;
}

JsonArray::~JsonArray()
{
  // Cleanup properties
  for (auto& el : elements) {
    delete el; // Release memory occupied by JsonValue objects
  }
  elements.clear(); // Clear the map
}

void JsonArray::addElement(JsonValue *value)
{
  elements.push_back(value);
}

const vector<JsonValue *> &JsonArray::getElements() const
{
  return elements;
}

JsonValue *JsonParser::parseJson(const string &json)
{
  size_t index = 0;
  return parseValue(json, index);
}

JsonValue *JsonParser::parseValue(const string &json, size_t &index)
{
  char firstChar = json[index];
  if (firstChar == '{') {
    return parseObject(json, index);
  } else if (firstChar == '[') {
    return parseArray(json, index);
  } else if (firstChar == '"') {
    return parseString(json, index);
  } else if (firstChar == '-' || (firstChar >= '0' && firstChar <= '9')) {
    return parseNumber(json, index);
  } else {
    return nullptr;
  }
}

JsonObject *JsonParser::parseObject(const string &json, size_t &index)
{
  JsonObject *object = new JsonObject();
  index++;

  while (json[index] != '}') {
    string key = parseString(json, index)->getValue();
    index++;
    JsonValue *value = parseValue(json, index);

    object->addProperty(key, value);

    if (json[index] == ',') {
      index++;
    }
  }

  index++;
  return object;
}

JsonArray *JsonParser::parseArray(const string &json, size_t &index)
{
  JsonArray *array = new JsonArray();
  index++;

  while (json[index] != ']') {
    JsonValue *value = parseValue(json, index);
    array->addElement(value);

    if (json[index] == ',') {
      index++;
    }
  }

  index++;
  return array;
}

JsonString *JsonParser::parseString(const string &json, size_t &index)
{
  index++;
  size_t start = index;
  while (json[index] != '"') {
    index++;
  }
  string value = json.substr(start, index - start);
  index++;
  return new JsonString(value);
}

JsonNumber *JsonParser::parseNumber(const string &json, size_t &index)
{
  size_t start = index;
  while (index < json.size() && (json[index] == '-' || (json[index] >= '0' && json[index] <= '9'))) {
    index++;
  }
  string value = json.substr(start, index - start);
  return new JsonNumber(stod(value));
}

void JsonPrinter::printJson(const JsonValue *jsonValue, bool prettyPrint, int indentation)
{
  if (jsonValue == nullptr) {
    cout << "null";
    return;
  }
  if (const JsonObject *jsonObject = dynamic_cast<const JsonObject *>(jsonValue)) {
    printJsonObject(jsonObject, prettyPrint, indentation);
  } else if (const JsonArray *jsonArray = dynamic_cast<const JsonArray *>(jsonValue)) {
    printJsonArray(jsonArray, prettyPrint, indentation);
  } else if (const JsonString *jsonString = dynamic_cast<const JsonString *>(jsonValue)) {
    printJsonString(jsonString);
  } else if (const JsonNumber *jsonNumber = dynamic_cast<const JsonNumber *>(jsonValue)) {
    printJsonNumber(jsonNumber);
  }
}

void JsonPrinter::printJsonObject(const JsonObject *jsonObject, bool prettyPrint, int indentation)
{
  cout << "{";
  if (prettyPrint && !jsonObject->getProperties().empty()) {
    cout << endl;
  }

  const auto &properties = jsonObject->getProperties();
  for (auto it = properties.begin(); it != properties.end(); ++it) {
    if (prettyPrint) {
      cout << setw(indentation + 2) << " ";
    }
    cout << '"' << it->first << "\":" << (prettyPrint ? " " : "");

    // Check for null value
    if (!it->second) {
      cout << "null";
    } else if (dynamic_cast<const JsonValue*>(it->second)) {
      printJson(it->second, prettyPrint, indentation + 2);
    }

    if (next(it) != properties.end()) {
      cout << ",";
    }
    if (prettyPrint) {
      cout << endl;
    }
  }

  if (prettyPrint && !jsonObject->getProperties().empty()) {
    cout << setw(indentation) << " ";
  }
  cout << "}";
}

void JsonPrinter::printJsonArray(const JsonArray *jsonArray, bool prettyPrint, int indentation)
{
  cout << "[";
  if (prettyPrint && !jsonArray->getElements().empty()) {
    cout << endl;
  }

  const auto &elements = jsonArray->getElements();
  for (auto it = elements.begin(); it != elements.end(); ++it) {
    if (prettyPrint) {
      cout << setw(indentation + 2) << " ";
    }

    // Check for null value
    if (*it == nullptr) {
      cout << "null";
    } else {
      printJson(*it, prettyPrint, indentation + 2);
    }

    if (next(it) != elements.end()) {
      cout << ",";
    }
    if (prettyPrint) {
      cout << endl;
    }
  }

  if (prettyPrint && !jsonArray->getElements().empty()) {
    cout << setw(indentation) << " ";
  }
  cout << "]";
}

void JsonPrinter::printJsonString(const JsonString *jsonString)
{
  cout << '"' << jsonString->getValue() << '"';
}

void JsonPrinter::printJsonNumber(const JsonNumber *jsonNumber)
{
  cout << jsonNumber->getValue();
}
