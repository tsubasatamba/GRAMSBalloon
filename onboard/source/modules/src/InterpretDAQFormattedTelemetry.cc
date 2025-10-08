#include "InterpretDAQFormattedTelemetry.hh"
using namespace anlnext;
namespace gramsballoon::pgrams {
ANLStatus InterpretDAQFormattedTelemetry::mod_define() {
  set_module_description("Interpret Telemetry encoded in DAQ format. This module is designed to be used as a basic class.");
  define_parameter("ReceiveTelemetry_name", &mod_class::receiveTelemetryName_);
  set_parameter_description("Name of the ReceiveTelemetry module");
  define_parameter("chatter", &mod_class::chatter_);
  define_parameter("print_telemetry", &mod_class::printTelemetry_);
  set_parameter_description("Print interpreted telemetry");
  return AS_OK;
}
ANLStatus InterpretDAQFormattedTelemetry::mod_initialize() {
  if (exist_module("SendTelemetry")) {
    get_module_NC("SendTelemetry", &sendTelemetry_);
  }
  else {
    std::cerr << module_name() << ": SendTelemetry module not found" << std::endl;
  }
  if (exist_module(receiveTelemetryName_)) {
    get_module_NC(receiveTelemetryName_, &receiveTelemetry_);
  }
  else {
    std::cerr << this->module_name() << ": ReceiveTelemetry module not found" << std::endl;
    if (sendTelemetry_) {
      sendTelemetry_->getErrorManager()->setError(ErrorType::MODULE_ACCESS_ERROR);
    }
    return AS_ERROR;
  }
  return AS_OK;
}
ANLStatus InterpretDAQFormattedTelemetry::mod_analyze() {
  if (!receiveTelemetry_) {
    return AS_ERROR;
  }
  if (!receiveTelemetry_->Valid()) {
    return AS_OK;
  }
  auto telemetry = receiveTelemetry_->Telemetry();
  if (!telemetry) {
    if (chatter_ >= 1) {
      std::cout << this->module_name() << ": telemetry is nullptr" << std::endl;
    }
    return AS_OK;
  }
  communicationFormat_ = telemetry->getContentsNC();
  if (!communicationFormat_) {
    if (chatter_ >= 1) {
      std::cout << this->module_name() << ": CommunicationFormat is nullptr" << std::endl;
    }
    return AS_OK;
  }
  communicationFormat_->interpret();
  if (printTelemetry_) {
    printTelemetry();
  }
  return AS_OK;
}
void InterpretDAQFormattedTelemetry::printTelemetry() const {
  std::cout << "Code: " << communicationFormat_->Code() << std::dec << std::endl;
  std::cout << "Argc: " << communicationFormat_->Argc() << std::endl;
  std::cout << "Arguments: ";
  const std::vector<int32_t> &args = communicationFormat_->Arguments();
  for (const auto &arg: args) {
    std::cout << arg << " ";
  }
  std::cout << std::endl;
}
} // namespace gramsballoon::pgrams