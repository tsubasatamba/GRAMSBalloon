#ifndef GRAMSBalloon_BaseTelemetryDefinition_hh
#define GRAMSBalloon_BaseTelemetryDefinition_hh 1
#include "CommunicationFormat.hh"
#include "fstream"
#include <cstdint>
#include <ctime>
#include <memory>
#include <ostream>
#include <regex>
#include <string>
#include <unordered_map>
#include <vector>
namespace gramsballoon::pgrams {

enum class Subsystem : uint16_t {
  HUB = 0,
  ORC = 1,
  COL = 2,
  QM = 3,
  TOF = 4,
  UNKNOWN = 255
};
class BaseTelemetryDefinition {
public:
  BaseTelemetryDefinition();
  virtual ~BaseTelemetryDefinition() = default;

private:
  std::shared_ptr<CommunicationFormat> contents_ = nullptr;
  std::time_t timeStamp_ = 0;
  Subsystem subsystem_ = Subsystem::UNKNOWN;
  uint32_t index_ = 0;
  std::unique_ptr<std::regex> reg_ = nullptr;

protected:
  void setType(Subsystem s) { subsystem_ = s; }
  void setArgc(uint16_t argc) {
    if (!contents_) {
      std::cerr << "Contents is not set!" << std::endl;
      return;
    }
    return contents_->setArgc(argc);
  }
  void setArguments(uint16_t index, int32_t argument) {
    if (!contents_) {
      std::cerr << "Contents is not set!" << std::endl;
      return;
    }
    return contents_->setArguments(index, argument);
  }

public:
  std::string construct();
  bool setContents(const std::shared_ptr<CommunicationFormat> &contents) {
    contents_ = contents;
    return true;
  }
  const CommunicationFormat *getContents() const { return contents_.get(); }
  CommunicationFormat *getContentsNC() const { return contents_.get(); }
  const std::shared_ptr<CommunicationFormat> &getContentsSptr() const { return contents_; }
  bool parseJSON(const std::string &jsonString);
  void setCurrentTime() {
    timeStamp_ = time(nullptr);
  }
  static std::string getTimeString(std::time_t t);
  virtual bool interpret(const std::shared_ptr<BaseTelemetryDefinition> &telemDef) {
    contents_ = telemDef->contents_;
    timeStamp_ = telemDef->timeStamp_;
    subsystem_ = telemDef->subsystem_;
    index_ = telemDef->index_;
    return true;
  }
  virtual void update() {
    if (contents_)
      contents_->update();
    index_++;
  };
  virtual std::ostream &print(std::ostream &stream) {
    stream << "BaseTelemetryDefinition" << std::endl;
    stream << "Time: " << timeStamp_ << std::endl;
    stream << "Index: " << index_ << std::endl;
    stream << "Subsystem" << static_cast<int>(subsystem_) << std::endl;
    return contents_->print(stream);
  }
  template <typename Iter, size_t N = 4>
  uint32_t compileValues(Iter iter, Iter end) {
    static_assert(N <= 4 && N > 0);
    uint32_t value = 0;
    for (size_t i = 0; i < N; i++) {
      if (iter == end) {
        break;
      }
      value += (*iter << (8 * (N - 1 - i)));
      iter++;
    }
    return value;
  }
  virtual std::ofstream &write(std::ofstream &stream) {
    stream << construct();
    return stream;
  }
};
} // namespace gramsballoon::pgrams
#endif // GRAMSBalloon_BaseTelemetryDefinition_hh