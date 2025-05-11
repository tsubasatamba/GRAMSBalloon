#include "SocketCommunicationManager.hh"
using namespace anlnext;
namespace gramsballoon::pgrams {
ANLStatus SocketCommunicationManager::mod_define() {
  set_module_description("This module is responsible for socket communication, mainly between the DAQ computer");
  define_parameter("ip", &mod_class::ip_);
  set_parameter_description("ip address of the server");
  define_parameter("port", &mod_class::port_);
  set_parameter_description("port of the server");
  define_parameter("handle_SIGPIPE", &mod_class::handleSigpipe_);
  set_parameter_description("Flag for handling SIGPIPE. This process is normally necessary once per process.");
  define_parameter("timeout", &mod_class::timeout_);
  set_parameter_description("Receive Timeout");
  set_parameter_unit(1.0, "usec");
  define_parameter("chatter", &mod_class::chatter_);
  return AS_OK;
}
ANLStatus SocketCommunicationManager::mod_pre_initialize() {
  if (handleSigpipe_)
    SocketCommunication::HandleSIGPIPE();
  return AS_OK;
}
ANLStatus SocketCommunicationManager::mod_initialize() {
  if (exist_module("SendTelemetry")) {
    get_module_NC("SendTelemetry", &sendTelemetry_);
  }
  else {
    std::cerr << "SocketCommunicationManager::mod_initialize SendTelemetry module is not found." << std::endl;
  }
  socketCommunication_ = std::make_shared<SocketCommunication>(port_);

  if (socketCommunication_->accept() != 0) {
    std::cerr << "SocketCommunicationManager::mod_initialize: Failed to connect to the server." << std::endl;
    socketCommunication_->close();
    if (sendTelemetry_) {
      sendTelemetry_->getErrorManager()->setError(ErrorType::SEND_TELEMETRY_SERIAL_COMMUNICATION_ERROR); // TODO: This flags is to be defined
    }
    return AS_ERROR;
  }
  return AS_OK;
}
ANLStatus SocketCommunicationManager::mod_analyze() {
  if (socketCommunication_ && socketCommunication_->isFailed()) {
    std::cout << "SocketCommunicationManager::mod_analyze: reconnecting socket communication..." << std::endl;
    if (socketCommunication_->accept() != 0) {
      std::cerr << "SocketCommunicationManager::mod_initialize: Failed to connect to the server." << std::endl;
      socketCommunication_->close();
      if (sendTelemetry_) {
        sendTelemetry_->getErrorManager()->setError(ErrorType::SEND_TELEMETRY_SERIAL_COMMUNICATION_ERROR); // TODO: This flags is to be defined
      }
      return AS_ERROR;
    }
  }
  return AS_OK;
}
ANLStatus SocketCommunicationManager::mod_finalize() {
  if (socketCommunication_) {
    socketCommunication_->close();
  }
  return AS_OK;
}
} // namespace gramsballoon::pgrams