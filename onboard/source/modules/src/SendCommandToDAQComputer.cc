#include "SendCommandToDAQComputer.hh"
using namespace anlnext;
namespace gramsballoon::pgrams {
ANLStatus SendCommandToDAQComputer::mod_define() {
  define_parameter("SocketCommunicationManager_name", &mod_class::socketCommunicationManagerName_);
  set_parameter_description("Name of SocketCommunicationManager");
  define_parameter("DistributeCommand_name", &mod_class::distributeCommandName_);
  set_parameter_description("Name of DistributeCommand");
  define_parameter("duration_between_heartbeat", &mod_class::durationBetweenHeartbeat_);
  set_parameter_description("Duration between heartbeat in ms");
  set_parameter_unit(1.0, "ms");
  define_parameter("chatter", &mod_class::chatter_);
  set_parameter_description("Chatter level");
  return AS_OK;
}
ANLStatus SendCommandToDAQComputer::mod_initialize() {
  if (exist_module("SendTelemetry")) {
    get_module_NC("SendTelemetry", &sendTelemetry_);
  }
  else {
    std::cerr << module_id() << "::mod_initialize SendTelemetry module is not found." << std::endl;
  }
  if (exist_module(socketCommunicationManagerName_)) {
    get_module_NC(socketCommunicationManagerName_, &socketCommunicationManager_);
  }
  else {
    std::cerr << module_id() << "::mod_initialize SocketCommunicationManager module is not found." << std::endl;
    if (sendTelemetry_) {
      sendTelemetry_->getErrorManager()->setError(ErrorType::MODULE_ACCESS_ERROR);
    }
  }
  if (exist_module(distributeCommandName_)) {
    get_module_NC(distributeCommandName_, &distributeCommand_);
  }
  else {
    std::cerr << module_id() << "::mod_initialize DistributeCommand module is not found." << std::endl;
    if (sendTelemetry_) {
      sendTelemetry_->getErrorManager()->setError(ErrorType::MODULE_ACCESS_ERROR);
    }
  }
  durationBetweenHeartbeatChrono_ = std::make_shared<std::chrono::milliseconds>(durationBetweenHeartbeat_);
  heartbeat_ = std::make_shared<CommunicationFormat>();
  if (heartbeat_) {
    heartbeat_->setCode(0xFFFF);
    heartbeat_->setArgc(1); // index of the heartbeat
    heartbeat_->setArguments(0, -1);
    heartbeat_->update();
  }
  else {
    std::cerr << module_id() << "::mod_initialize Failed to create CommunicationFormat." << std::endl;
    return AS_ERROR;
  }
  failed_ = false;
  return AS_OK;
}
ANLStatus SendCommandToDAQComputer::mod_analyze() {
  if (!socketCommunicationManager_) {
    std::cerr << module_id() << "::mod_analyze SocketCommunicationManager is nullptr." << std::endl;
    return AS_OK;
  }
  auto sc = socketCommunicationManager_->getSocketCommunication();
  if (!sc) {
    std::cerr << module_id() << "::mod_analyze SocketCommunication in the SocketCommunicationManager is nullptr." << std::endl;
    return AS_OK;
  }
  auto m_sptr = distributeCommand_->getAndPopPayload();
  if (sc->isConnected()) {
    if (chatter_ > 2) {
      std::cout << "SocketCommunication is connected." << std::endl;
    }
  }
  else {
    if (chatter_ > 2) {
      std::cout << "SocketCommunication is not connected." << std::endl;
    }
    failed_ = true;
    m_sptr.reset();
    return AS_OK;
  }
  if (!m_sptr) {
    auto now = std::chrono::high_resolution_clock::now();
    const bool need_heartbeat = (!lastTime_) || (lastTime_ && (now - *lastTime_) > *durationBetweenHeartbeatChrono_);
    if (!lastTime_) {
      lastTime_ = std::make_shared<std::chrono::time_point<std::chrono::high_resolution_clock>>(now);
    }
    if (need_heartbeat) {
      heartbeat_->setArguments(0, heartbeat_->getArguments(0) + 1);
      heartbeat_->update();
      if (chatter_ > 1) {
        std::cout << "Sending heartbeat (index: " << heartbeat_->getArguments(0) << ")" << std::endl;
      }
      if (chatter_ > 2) {
        for (const auto &byte: heartbeat_->Command()) {
          std::cout << std::hex << static_cast<int>(byte) << std::dec << " ";
        }
        std::cout << std::endl;
      }
      const int send_result = socketCommunicationManager_->sendAndWaitForAck(heartbeat_->Command());
      if (send_result < 0) {
        std::cerr << "Error in " << module_id() << "::mod_analyze: " << "Sending heartbeat is failed" << std::endl;
      }
      else if(chatter_ > 1) {
        std::cout << "Sent heartbeat" << std::endl;
      }
      *lastTime_ = now;
    }
    return AS_OK;
  }
  const int send_result = socketCommunicationManager_->sendAndWaitForAck(m_sptr->payload); // TODO: this depends on telemetry definition.
  if (send_result < 0) {
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
  return AS_OK;
}
} // namespace gramsballoon::pgrams
