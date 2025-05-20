#include "SendCommandToDAQComputer.hh"
using namespace anlnext;
namespace gramsballoon::pgrams {
ANLStatus SendCommandToDAQComputer::mod_define() {
  define_parameter("SocketCommunicationManager_name", &mod_class::socketCommunicationManagerName_);
  set_parameter_description("Name of SocketCommunicationManager");
  define_parameter("DistributeCommand_name", &mod_class::distributeCommandName_);
  set_parameter_description("Name of DistributeCommand");
  define_parameter("chatter", &mod_class::chatter_);
  set_parameter_description("Chatter level");
  return AS_OK;
}
ANLStatus SendCommandToDAQComputer::mod_initialize() {
  if (exist_module("SendTelemetry")) {
    get_module_NC("SendTelemetry", &sendTelemetry_);
  }
  else {
    std::cerr << "SendTelemetry module is not found." << std::endl;
  }
  if (exist_module(socketCommunicationManagerName_)) {
    get_module_NC(socketCommunicationManagerName_, &socketCommunicationManager_);
  }
  else {
    std::cerr << "SocketCommunicationManager module is not found." << std::endl;
    if (sendTelemetry_) {
      sendTelemetry_->getErrorManager()->setError(ErrorType::MODULE_ACCESS_ERROR);
    }
  }
  if (exist_module(distributeCommandName_)) {
    get_module_NC(distributeCommandName_, &distributeCommand_);
  }
  else {
    std::cerr << "DistributeCommand module is not found." << std::endl;
    if (sendTelemetry_) {
      sendTelemetry_->getErrorManager()->setError(ErrorType::MODULE_ACCESS_ERROR);
    }
  }
  failed_ = false;
  return AS_OK;
}
ANLStatus SendCommandToDAQComputer::mod_analyze() {
  if (!socketCommunicationManager_) {
    std::cerr << "SocketCommunicationManager is nullptr." << std::endl;
    return AS_OK;
  }
  auto sc = socketCommunicationManager_->getSocketCommunication();
  if (!sc) {
    std::cerr << "SocketCommunication in the SocketCommunicationManager is nullptr." << std::endl;
    return AS_OK;
  }
  auto m_sptr = distributeCommand_->getAndPopPayload();
  if (sc->isConnected()) {
    if (chatter_ > 0) {
      std::cout << "SocketCommunication is connected." << std::endl;
    }
  }
  else {
    if (chatter_ > 0) {
      std::cout << "SocketCommunication is not connected." << std::endl;
    }
    failed_ = true;
    m_sptr.reset();
    return AS_OK;
  }
  const auto send_result = socketCommunicationManager_->sendAndWaitForAck(m_sptr->payload); // TODO: this depends on telemetry definition.
  if (send_result == -1) {
    std::cerr << "Error in " << module_id() << "::mod_analyze: " << "Sending command is failed" << std::endl;
  }
  else if (chatter_ > 0) {
    std::cout << "Sent data from " << m_sptr->topic << std::endl;
    std::cout << "Payload size: " << send_result << std::endl;
  }
  if (chatter_ > 1) {
    std::cout << "Payload:" << std::endl;
    for (const auto &byte: m_sptr->payload) {
      std::cout << static_cast<int>(byte) << " ";
    }
    std::cout << std::endl;
  }
}
} // namespace gramsballoon::pgrams
