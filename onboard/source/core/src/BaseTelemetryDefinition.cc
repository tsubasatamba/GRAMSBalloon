#include "BaseTelemetryDefinition.hh"
#include "boost/format.hpp"
namespace gramsballoon::pgrams {
BaseTelemetryDefinition::BaseTelemetryDefinition() {
  jsonDoc_ = std::make_unique<rapidjson::Document>();
  buffer_ = std::make_unique<rapidjson::StringBuffer>();
  writer_ = std::make_unique<rapidjson::Writer<rapidjson::StringBuffer>>(*buffer_);
}

std::string BaseTelemetryDefinition::getTimeString(std::time_t t) {
  char buffer[20];
  std::strftime(buffer, sizeof(buffer), "%m%d%H%M%S", std::localtime(&t));
  return std::string(buffer);
}
bool BaseTelemetryDefinition::parseJSON(const std::string &jsonString) {
  if (!jsonDoc_) {
    jsonDoc_ = std::make_unique<rapidjson::Document>();
  }
  jsonDoc_->Parse(jsonString.c_str());
  if (jsonDoc_->HasParseError()) {
    return false;
  }
  bool is_success = true;
  for (const auto &[key, value]: data_) {
    if (jsonDoc_->HasMember(key.c_str()) && (*jsonDoc_)[key.c_str()].IsString()) {
      data_[key] = (*jsonDoc_)[key.c_str()].GetString();
    }
    else {
      data_[key] = ""; // Key not found or value is not a string
      is_success = false;
    }
  }
  return is_success;
}
bool BaseTelemetryDefinition::registerJSONKey(const std::string &key) {
  if (!jsonDoc_) {
    jsonDoc_ = std::make_unique<rapidjson::Document>();
  }
  if (jsonDoc_->HasMember(key.c_str())) {
    return false; // Key already exists
  }
  data_[key] = "";
  jsonDoc_->AddMember(rapidjson::Value(key.c_str(), jsonDoc_->GetAllocator()).Move(), "", jsonDoc_->GetAllocator());
  return true;
}
std::string BaseTelemetryDefinition::convert2JSON() const {
  if (!jsonDoc_) {
    return "{}"; // Return empty JSON if no document is available
  }
  if (!buffer_) {
    return "{}";
  }
  if (!writer_) {
    return "{}";
  }
  jsonDoc_->Accept(*writer_);
  return std::string(buffer_->GetString());
}
} // namespace gramsballoon::pgrams