#include "MQTTFormatConverter.hh"
#include <chrono>
#include <sstream>
namespace gramsballoon::pgrams {
MQTTFormatConverter::MQTTFormatConverter() {
}
std::string MQTTFormatConverter::Convert(time_t t, const CommunicationFormat &comdef) {
  std::string result = "{";
  result += ConstructOneItem("t", t) + ",";
  result += ConstructOneItem("c", comdef.Code()) + ",";
  result += ConstructOneItem("n", comdef.Argc()) + ",";
  result += ConstructOneItem("d", comdef.Arguments());
  result += "}";
  return result;
}
} // namespace gramsballoon::pgrams