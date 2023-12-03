#include "VortexJson.h"
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;

JsonValue *JsonParser::parseJson(const std::string &json)
{
  std::istringstream stream(json);
  return parseFromStream(stream);
}

JsonValue *JsonParser::parseJsonFromFile(const std::string &filename)
{
  std::ifstream file(filename);
  if (!file.is_open()) {
    std::cerr << "Error opening file: " << filename << std::endl;
    return nullptr;
  }

  return parseFromStream(file);
}

JsonValue *JsonParser::parseFromStream(std::istream &stream)
{
  std::string json;
  char ch;
  while (stream.get(ch)) {
    json += ch;
  }

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
  if (!object) {
    return nullptr;
  }
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
  if (!array) {
    return nullptr;
  }
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

void JsonPrinter::printJson(const JsonValue *jsonValue, bool prettyPrint)
{
  printJsonInternal(jsonValue, prettyPrint, 0);
}

void JsonPrinter::writeJson(std::ofstream &file, const JsonValue *jsonValue, bool prettyPrint)
{
  writeToStream(file, jsonValue, prettyPrint, 0);
}

void JsonPrinter::printJsonInternal(const JsonValue *jsonValue, bool prettyPrint, int indentation)
{
  writeToStream(std::cout, jsonValue, prettyPrint, indentation);
}

void JsonPrinter::writeToStream(std::ostream &stream, const JsonValue *jsonValue, bool prettyPrint, int indentation) {
  if (jsonValue == nullptr) {
    stream << "null";
    return;
  }

  switch (jsonValue->getType()) {
    case JsonValueType::Object:
      printJsonObject(stream, (const JsonObject *)jsonValue, prettyPrint, indentation);
      break;
    case JsonValueType::Array:
      printJsonArray(stream, (const JsonArray *)jsonValue, prettyPrint, indentation);
      break;
    case JsonValueType::String:
      printJsonString(stream, (const JsonString *)jsonValue);
      break;
    case JsonValueType::Number:
      printJsonNumber(stream, (const JsonNumber *)jsonValue);
      break;
    default:
      stream << "null";
      break;
  }
}

void JsonPrinter::printJsonObject(std::ostream &stream, const JsonObject *jsonObject, bool prettyPrint, int indentation) {
  stream << "{";
  if (prettyPrint && !jsonObject->getProperties().empty()) {
    stream << std::endl;
  }

  const auto &properties = jsonObject->getProperties();
  for (auto it = properties.begin(); it != properties.end(); ++it) {
    if (prettyPrint) {
      stream << std::setw(indentation + 2) << " ";
    }
    stream << '"' << it->first << "\":" << (prettyPrint ? " " : "");

    // Directly write the property value to the stream
    writeToStream(stream, it->second, prettyPrint, indentation + 2);

    if (std::next(it) != properties.end()) {
      stream << ",";
    }
    if (prettyPrint) {
      stream << std::endl;
    }
  }

  if (prettyPrint && !jsonObject->getProperties().empty()) {
    stream << std::setw(indentation) << " ";
  }
  stream << "}";
}

void JsonPrinter::printJsonArray(std::ostream &stream, const JsonArray *jsonArray, bool prettyPrint, int indentation)
{
  stream << "[";
  if (prettyPrint && !jsonArray->getElements().empty()) {
    stream << std::endl;
  }

  const auto &elements = jsonArray->getElements();
  for (auto it = elements.begin(); it != elements.end(); ++it) {
    if (prettyPrint) {
      stream << std::setw(indentation + 2) << " ";
    }

    // Check for null value
    if (*it == nullptr) {
      stream << "null";
    } else {
      writeToStream(stream, *it, prettyPrint, indentation + 2);
    }

    if (next(it) != elements.end()) {
      stream << ",";
    }
    if (prettyPrint) {
      stream << std::endl;
    }
  }

  if (prettyPrint && !jsonArray->getElements().empty()) {
    stream << std::setw(indentation) << " ";
  }
  stream << "]";
}

void JsonPrinter::printJsonString(std::ostream &stream, const JsonString *jsonString)
{
  stream << '"' << jsonString->getValue() << '"';
}

void JsonPrinter::printJsonNumber(std::ostream &stream, const JsonNumber *jsonNumber)
{
  stream << jsonNumber->getValue();
}
