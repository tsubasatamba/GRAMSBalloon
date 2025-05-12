#include "ReceiveStatusFromDAQComputer.hh"
#include <iomanip>
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
  if (exist_module(socketCommunicationManagerName_)) {
    get_module_NC(socketCommunicationManagerName_, &socketCommunicationManager_);
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
  if (!sc->isConnected()) {
    std::cerr << "ReceiveStatusFromDAQComputer::mod_analyze(): Socket Communication is not connected." << std::endl;
    return AS_OK;
  }
  std::vector<uint8_t> *buffer_for_display = nullptr;
  if (chatter_ > 1) {
    buffer_for_display = new std::vector<uint8_t>;
    buffer_for_display->reserve(MAX_BYTES);
  }
  const auto result = sc->receive(bufTmp_);
  if (result > 0) {
    buffer_.insert(buffer_.end(), bufTmp_.begin(), bufTmp_.begin() + result);
    if (chatter_ > 1) {
      buffer_for_display->insert(buffer_for_display->end(), bufTmp_.begin(), bufTmp_.begin() + result);
    }
  }
  else if (result != 0) {
    if (chatter_ > 1) {
      std::cout << "Error in ReceiveStatusFromDAQComputer::mod_analyze: receiving data failed. error code = " << errno << "(" << strerror(errno) << ")" << std::endl;
    }
  }
  if (chatter_ > 0) {
    std::cout << "Received " << result << " bytes." << std::endl;
  }
  if (chatter_ > 1) {
    std::cout << "Payload:" << std::endl;
    for (const auto &byte: *buffer_for_display) {
      std::cout << std::hex << static_cast<int>(byte) << std::dec << " ";
    }
    std::cout << std::endl;
  }
  return AS_OK;
}
ANLStatus ReceiveStatusFromDAQComputer::mod_finalize() {
  return AS_OK;
}
} // namespace gramsballoon::pgrams
