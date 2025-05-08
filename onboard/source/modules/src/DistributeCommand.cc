#include "DistributeCommand.hh"
#include <errno.h>
#include <iostream>
#include <netinet/in.h>
#include <signal.h>
#include <string>
using namespace anlnext;

namespace gramsballoon::pgrams {
ANLStatus DistributeCommand::mod_define() {
  define_parameter("SocketCommunicationManager_name", &mod_class::socketCommunicationManagerName_);
  set_parameter_description("Name of SocketCommunicationManager");
  define_parameter("topic", &mod_class::topic_);
  set_parameter_description("Topic to subscribe");
  define_parameter("chatter", &mod_class::chatter_);
  return AS_OK;
}
ANLStatus DistributeCommand::mod_initialize() {
  if (exist_module("MosquittoManager")) {
    get_module_NC("MosquittoManager", &mosquittoManager_);
  }
  else {
    std::cerr << "MosquittoManager module is not found." << std::endl;
    return AS_ERROR;
  }
  auto mosq = mosquittoManager_->getMosquittoIO();
  if (!mosq) {
    std::cerr << "MosquittoIO in the MosquittoManager is nullptr." << std::endl;
    return AS_ERROR;
  }

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
    return AS_ERROR; // TODO: This is to be considered
  }
  ///// Initialize sockets
  mosq->subscribe(NULL, topic_.c_str(), 0);
  failed_ = false;
  auto sc = socketCommunicationManager_->getSocketCommunication();
  if (chatter_ > 0) {
    std::cout << "Connected to " << topic_ << std::endl;
  }
  if (chatter_ > 1) {
    std::cout << "IP: " << sc->getIP() << std::endl;
    std::cout << "Port: " << sc->getPort() << std::endl;
  }
  if (chatter_ > 2) {
    std::cout << "Socket: " << sc->getSocket() << std::endl;
  }
  return AS_OK;
}

ANLStatus DistributeCommand::mod_analyze() {
  if (!socketCommunicationManager_) {
    std::cerr << "SocketCommunicationManager is nullptr." << std::endl;
    return AS_OK;
  }
  auto sc = socketCommunicationManager_->getSocketCommunication();
  if (!sc) {
    std::cerr << "SocketCommunication in the SocketCommunicationManager is nullptr." << std::endl;
    return AS_OK;
  }
  auto mosq = mosquittoManager_->getMosquittoIO();
  if (!mosq) {
    std::cerr << "MosquittoIO in the MosquittoManager is nullptr." << std::endl;
    return AS_OK;
  }
  auto commands = mosq->getPayload();
  if (commands.empty()) {
    return AS_OK;
  }
  // need to lock the mutex here or use a thread-safe queue
  const auto command = commands.front();
  const auto &command_payload = command->payload;
  if (topic_ == command->topic) {
    if (chatter_ > 0) {
      std::cout << "Received command for " << topic_ << std::endl;
    }
    if (sc->isOpened()) {
      std::cerr << "Socket is not opened." << std::endl;
      mosq->popPayloadFront();
      return AS_OK;
    }
    for (int i = 0; i < numTrial_; i++) {
      const auto send_result = sc->send(command_payload); // TODO: this depends on telemetry definition.
      if (send_result == -1) {
        std::cerr << "Error in DistributeCommand::mod_analyze: " << "Trial " << i << " Sending data failed." << std::endl;
        continue;
      }
      else if (chatter_ > 0) {
        std::cout << "Sent data to " << topic_ << std::endl;
        std::cout << "Payload size: " << send_result << std::endl;
      }
      failed_ = false;
      break;
    }
    if (failed_) {
      std::cerr << "Error in DistributeCommand::mod_analyze: Sending data failed, despite " << numTrial_ << " times trials." << std::endl;
    }
    else {
      if (chatter_ > 1) {
        std::cout << "Payload:" << std::endl;
        for (const auto &byte: command_payload) {
          std::cout << static_cast<int>(byte) << " ";
        }
        std::cout << std::endl;
      }
    }
    mosq->popPayloadFront(); // TODO: How to handle the case when the data is not sent?
  }
  return AS_OK;
}
ANLStatus DistributeCommand::mod_finalize() {
  return AS_OK;
}
} // namespace gramsballoon::pgrams