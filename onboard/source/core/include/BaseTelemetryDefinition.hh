#ifndef GRAMSBalloon_BaseTelemetryDefinition_hh
#define GRAMSBalloon_BaseTelemetryDefinition_hh 1
#include "CommunicationFormat.hh"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include <cstdint>
#include <ctime>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
namespace gramsballoon::pgrams {
class BaseTelemetryDefinition {
public:
  BaseTelemetryDefinition();
  virtual ~BaseTelemetryDefinition() = default;

private:
  const CommunicationFormat *contents_;
  std::time_t timeStamp_ = 0;
  std::unique_ptr<rapidjson::Document> jsonDoc_ = nullptr;
  std::unique_ptr<rapidjson::StringBuffer> buffer_ = nullptr;
  std::unique_ptr<rapidjson::Writer<rapidjson::StringBuffer>> writer_ = nullptr;
  std::unordered_map<std::string, std::string> data_;

public:
  bool setContents(const CommunicationFormat *contents) {
    contents_ = contents;
    return true;
  }
  void setCurrentTime() {
    timeStamp_ = time(nullptr);
  }
  bool parseJSON(const std::string &jsonString);
  bool registerJSONKey(const std::string &key);
  static std::string getTimeString(std::time_t t);
  std::string getDataFromKey(const std::string &key) const {
    return data_.count(key) ? data_.at(key) : "";
  }
  std::string convert2JSON() const;
};
} // namespace gramsballoon::pgrams
#endif // GRAMSBalloon_BaseTelemetryDefinition_hh