#include "BaseTelemetryDefinition.hh"
#include "CommunicationCodes.hh"
#include <iomanip>
#include <sstream>
namespace gramsballoon::pgrams {
BaseTelemetryDefinition::BaseTelemetryDefinition() {
  contents_ = std::make_shared<CommunicationFormat>();
  reg_ = std::make_unique<std::regex>("\\{\"t\":\"([0-9]{10})\",\"s\":\"([hocqt])\",\"i\":([0-9]{6}),\"c\":\"([\\x00-\\xff]+)\"\\}");
}
std::string BaseTelemetryDefinition::construct() {
  std::string subsystem_str;
  switch (subsystem_) {
  case Subsystem::HUB:
    subsystem_str = "h";
    break;
  case Subsystem::ORC:
    subsystem_str = "o";
    break;
  case Subsystem::COL:
    subsystem_str = "c";
    break;
  case Subsystem::QM:
    subsystem_str = "q";
    break;
  case Subsystem::TOF:
    subsystem_str = "t";
    break;
  default:
    subsystem_str = "u";
    break;
  }
  std::string content = "";
  if (contents_) {
    contents_->update();
    content = contents_->CommandStr();
  }
  std::stringstream ss;
  ss << "{\"t\":\"" << getTimeString(timeStamp_) << "\",\"s\":\"" << subsystem_str << "\",\"i\":" << std::setfill('0') << std::setw(6) << index_ << std::setw(0) << std::setfill(' ') << ",\"c\":\"" << content << "\"}";
  return ss.str();
}
std::string BaseTelemetryDefinition::getTimeString(std::time_t t) {
  char buffer[20];
  std::strftime(buffer, sizeof(buffer), "%m%d%H%M%S", std::localtime(&t));
  return std::string(buffer);
}
bool BaseTelemetryDefinition::parseJSON(const std::string &jsonString) {
  bool is_success = true;
  const size_t sz = jsonString.size();
  if (sz < 44) {
    std::cerr << "BaseTelemetryDefinition::parseJSON error: jsonString is too short" << std::endl;
    return false;
  }
  std::smatch match;
  if (!std::regex_match(jsonString, match, *reg_)) {
    std::cerr << "BaseTelemetryDefinition::parseJSON error: regex match failed" << std::endl;
    return false;
  }
  if (match.size() != 5) {
    std::cerr << "BaseTelemetryDefinition::parseJSON error: match size is not 5" << std::endl;
    return false;
  }
  try {
    timeStamp_ = std::stol(match[1].str());
  }
  catch (const std::exception &e) {
    std::cerr << "BaseTelemetryDefinition::parseJSON error: time conversion failed: " << e.what() << std::endl;
    is_success = false;
  }
  try {
    const std::string s_str = match[2].str();
    if (s_str == "h") {
      subsystem_ = Subsystem::HUB;
    }
    else if (s_str == "o") {
      subsystem_ = Subsystem::ORC;
    }
    else if (s_str == "c") {
      subsystem_ = Subsystem::COL;
    }
    else if (s_str == "q") {
      subsystem_ = Subsystem::QM;
    }
    else if (s_str == "t") {
      subsystem_ = Subsystem::TOF;
    }
    else {
      subsystem_ = Subsystem::UNKNOWN;
      is_success = false;
    }
  }
  catch (const std::exception &e) {
    std::cerr << "BaseTelemetryDefinition::parseJSON error: subsystem conversion failed: " << e.what() << std::endl;
    is_success = false;
  }
  try {
    index_ = std::stol(match[3].str());
  }
  catch (const std::exception &e) {
    std::cerr << "BaseTelemetryDefinition::parseJSON error: index conversion failed: " << e.what() << std::endl;
    is_success = false;
  }
  try {
    const std::string c_str = match[4].str();
    contents_->setData(c_str);
    contents_->interpret();
  }
  catch (const std::exception &e) {
    std::cerr << "BaseTelemetryDefinition::parseJSON error: command conversion failed: " << e.what() << std::endl;
    is_success = false;
  }
  return is_success;
}
} // namespace gramsballoon::pgrams