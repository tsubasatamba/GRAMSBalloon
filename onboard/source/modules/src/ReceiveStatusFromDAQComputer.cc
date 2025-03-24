#include "ReceiveStatusFromDAQComputer.hh"
using namespace anlnext;
namespace gramsballoon::pgrams {
ANLStatus ReceiveStatusFromDAQComputer::mod_define() {
  set_module_description("This module is responsible for receiving status from DAQ computers. SocketCommunicationManager is required for communication.");
  define_parameter("SocketCommunicationManager_name", &mod_class::socketCommunicationManagerName_);
  set_parameter_description("Name of SocketCommunicationManager");
  define_parameter("chatter", &mod_class::chatter_);
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
  if (!socketCommunicationManager_) {
    std::cerr << "ReceiveStatusFromDAQComputer::mod_analyze: SocketCommunicationManager is nullptr." << std::endl;
    return AS_OK;
  }
  auto sc = socketCommunicationManager_->getSocketCommunication();
  if (!sc) {
    std::cerr << "ReceiveStatusFromDAQComputer::mod_analyze: SocketCommunication in the SocketCommunicationManager is nullptr." << std::endl;
    return AS_OK;
  }
  if (sc->isFailed()) {
    std::cerr << "Socket Communication is failed." << std::endl;
    return AS_OK;
  }
  uint8_t byte;
  std::vector<uint8_t> *buffer_for_display = nullptr;
  if (chatter_ > 1) {
    buffer_for_display = new std::vector<uint8_t>;
    buffer_for_display->reserve(MAX_BYTES);
  }
  int num_bytes = 0;
  for (int i = 0; i < MAX_BYTES; i++) {
    const auto result = sc->receive(byte);
    if (result > 0) {
      buffer_.push(byte);
      num_bytes++;
      if (chatter_ > 1) {
        buffer_for_display->push_back(byte);
      }
    }
    else {
      if (chatter_ > 1) {
        std::cout << "Error in ReceiveStatusFromDAQComputer::mod_analyze: receiving data failed. error code = " << errno << "(" << strerror(errno) << ")" << std::endl;
      }
      break;
    }
  }
  if (chatter_ > 0) {
    std::cout << "Received " << num_bytes << " bytes." << std::endl;
  }
  if (chatter_ > 1) {
    std::cout << "Payload:" << std::endl;
    for (const auto &byte: *buffer_for_display) {
      std::cout << static_cast<int>(byte) << " ";
    }
    std::cout << std::endl;
  }
  return AS_OK;
}
ANLStatus ReceiveStatusFromDAQComputer::mod_finalize() {
  return AS_OK;
}
} // namespace gramsballoon::pgrams
