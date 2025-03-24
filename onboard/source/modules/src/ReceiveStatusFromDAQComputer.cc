#include "ReceiveStatusFromDAQComputer.hh"
using namespace anlnext;
namespace gramsballoon::pgrams {
ANLStatus ReceiveStatusFromDAQComputer::mod_define() {
  set_module_description("This module is responsible for receiving status from DAQ computers. SocketCommunicationManager is required for communication.");
  define_parameter("SocketCommunicationManager_name", &mod_class::socketCommunicationManagerName_);
  set_parameter_description("Name of SocketCommunicationManager");
  return AS_OK;
}
ANLStatus ReceiveStatusFromDAQComputer::mod_initialize() {
  if (exist_module("SendTelemetry")) {
    get_module_NC("SendTelemetry", &sendTelemetry_);
  }
  else {
    std::cerr << "Error in ReceiveStatusFromDAQComputer::mod_initialize: SendTelemetry not found." << std::endl;
  }
  if (exist_module("SocketCommunicationManager")) {
    get_module_NC("SocketCommunicationManager", &socketCommunicationManager_);
  }
  else {
    std::cerr << "Error in ReceiveStatusFromDAQComputer::mod_initialize: SocketCommunicationManager not found." << std::endl;
    if (sendTelemetry_) {
      sendTelemetry_->getErrorManager()->setError(ErrorType::MODULE_ACCESS_ERROR);
    }
  }
  return AS_OK;
}
ANLStatus ReceiveStatusFromDAQComputer::mod_analyze() {
  return AS_OK;
}
ANLStatus ReceiveStatusFromDAQComputer::mod_finalize() {
  return AS_OK;
}
} // namespace gramsballoon::pgrams
