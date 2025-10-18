#include "SocketCommunicationManager.hh"
#include <sys/select.h>
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
  set_parameter_unit(1.0, "msec");
  define_parameter("ack_type", &mod_class::ackTypeInt_);
  set_parameter_description("Acknowledgement type. 0: Size, 1: Raw, 2: None");
  define_parameter("subsystem", &mod_class::subsystemInt_);
  define_parameter("chatter", &mod_class::chatter_);
  return AS_OK;
}
ANLStatus SocketCommunicationManager::mod_pre_initialize() {
  if (handleSigpipe_)
    SocketCommunication::HandleSIGPIPE();
  return AS_OK;
}
ANLStatus SocketCommunicationManager::mod_initialize() {
  subsystem_ = magic_enum::enum_cast<Subsystem>(subsystemInt_).value_or(Subsystem::UNKNOWN);
  if (subsystem_ == Subsystem::UNKNOWN) {
    std::cerr << "Error in " << module_id() << "::mod_initialize: Invalid subsystem value " << subsystemInt_ << ". Setting to UNKNOWN." << std::endl;
    if (sendTelemetry_) {
      sendTelemetry_->getErrorManager()->setError(ErrorType::OTHER_ERRORS);
    }
  }
  if (exist_module("SendTelemetry")) {
    get_module_NC("SendTelemetry", &sendTelemetry_);
  }
  else {
    std::cerr << module_id() << "::mod_initialize SendTelemetry module is not found." << std::endl;
  }
  if (exist_module("IoContextManager")) {
    get_module_NC("IoContextManager", &ioContextManager_);
    ioContext_ = ioContextManager_->getIoContext();
    if (!ioContext_) {
      std::cerr << module_id() << "::mod_initialize IoContextManager is not initialized." << std::endl;
      return AS_ERROR;
    }
  }
  else {
    std::cerr << module_id() << "::mod_initialize IoContextManager module is not found." << std::endl;
    if (sendTelemetry_) {
      sendTelemetry_->getErrorManager()->setError(ErrorType::MODULE_ACCESS_ERROR);
    }
    return AS_ERROR;
  }
  try {
    socketCommunication_ = std::make_shared<SocketCommunication>(ioContext_, port_);
  }
  catch (...) {
    socketCommunication_ = nullptr;
  }
  if (!socketCommunication_) {
    std::cerr << module_id() << "::mod_initialize SocketCommunication is not initialized." << std::endl;
    if (sendTelemetry_) {
      sendTelemetry_->getErrorManager()->setError(ErrorManager::GetDaqComErrorType(subsystem_, true));
    }
    return AS_ERROR;
  }
  if (ackTypeInt_ == 0) {
    ackType_ = AcknowledgementType::SIZE;
  }
  else if (ackTypeInt_ == 1) {
    ackType_ = AcknowledgementType::RAW;
  }
  else if (ackTypeInt_ == 2) {
    ackType_ = AcknowledgementType::NONE;
  }
  else {
    std::cerr << module_id() << "::mod_initialize Invalid ack_type: " << ackTypeInt_ << ". Setting to NONE." << std::endl;
    ackType_ = AcknowledgementType::NONE;
  }
  if (timeout_ > 0) {
    std::cout << module_id() << "::mod_initialize Timeout is set to " << timeout_ << " msec." << std::endl;
    socketCommunication_->setTimeout(timeout_);
  }
  socketCommunication_->accept();
  return AS_OK;
}
ANLStatus SocketCommunicationManager::mod_analyze() {
  return AS_OK;
}
ANLStatus SocketCommunicationManager::mod_finalize() {
  if (socketCommunication_) {
    socketCommunication_->close();
  }
  return AS_OK;
}
int SocketCommunicationManager::sendAndWaitForAck(const uint8_t *buf, size_t n) {
  const int send_result = socketCommunication_->send(buf, n);
  if (send_result < 0) {
    return send_result;
  }
  if (chatter_ > 2) {
    std::cout << module_id() << "::sendAndWaitForAck: Sent data: ";
    for (size_t i = 0; i < n; ++i) {
      std::cout << static_cast<int>(buf[i]) << " ";
    }
    std::cout << std::endl;
  }
  ackBuffer_.clear();
  if (ackType_ == AcknowledgementType::SIZE) {
    const int ret = socketCommunication_->receiveWithTimeout(ackBuffer_);
    if (ret < 0) {
      return ret;
    }
    else if (ret != sizeof(uint16_t)) {
      std::cerr << module_id() << "::sendAndWaitForAck: Error in receiving acknowledgement. Expected size: " << sizeof(uint8_t) << ", Received size: " << ret << std::endl;
      std::cerr << module_id() << "::sendAndWaitForAck: Acknowledgement data: ";
      for (int i = 0; i < ret; ++i) {
        std::cerr << static_cast<int>(ackBuffer_[i]) << " ";
      }
      std::cerr << std::endl;
      return -ret;
    }
    if (chatter_ > 2) {
      std::cout << module_id() << "::sendAndWaitForAck: Received acknowledgement: ";
      for (const auto &byte: ackBuffer_) {
        std::cout << static_cast<int>(byte) << " ";
      }
      std::cout << std::endl;
    }
    uint16_t packetsz = (ackBuffer_[0] << 8) | ackBuffer_[1];
    if (packetsz != n) {
      std::cerr << module_id() << "::sendAndWaitForAck: Acknowledgement size mismatch. Expected: " << n << ", Received: " << packetsz << std::endl;
      return -ackBuffer_[0];
    }
  }
  else if (ackType_ == AcknowledgementType::RAW) {
    const int ret = socketCommunication_->receiveWithTimeout(ackBuffer_);
    if (ret < 0) {
      return ret;
    }
    if (chatter_ > 2) {
      std::cout << module_id() << "::sendAndWaitForAck: Received acknowledgement: ";
      for (const auto &byte: ackBuffer_) {
        std::cout << static_cast<int>(byte) << " ";
      }
      std::cout << std::endl;
    }
    const size_t acksz = ackBuffer_.size();
    if (acksz != n) {
      std::cerr << module_id() << "::sendAndWaitForAck: Acknowledgement size mismatch. Expected: " << n << ", Received: " << ackBuffer_.size() << std::endl;
      return -acksz;
    }
    bool failed = false;
    for (size_t i = 0; i < acksz; ++i) {
      if (ackBuffer_[i] != buf[i]) {
        std::cerr << module_id() << "::sendAndWaitForAck: Acknowledgement data mismatch at " << i << ". Expected: " << static_cast<int>(buf[i]) << ", Received: " << static_cast<int>(ackBuffer_[i]) << std::endl;
        failed = true;
      }
    }
    if (failed) {
      return -1;
    }
  }
  return send_result;
}
int SocketCommunicationManager::receiveAndSendAck(std::vector<uint8_t> &data) {
  const int ret = socketCommunication_->receive(data);
  if (ret <= 0) {
    return ret;
  }
  data.resize(ret);
  if (ackType_ == AcknowledgementType::SIZE) {
    ackBuffer_.resize(sizeof(uint16_t));
    ackBuffer_[0] = (static_cast<uint16_t>(ret) >> 8) & 0x00FF;
    ackBuffer_[1] = static_cast<uint16_t>(ret) & 0x00FF;
    socketCommunication_->send(ackBuffer_.data(), sizeof(uint16_t));
  }
  else if (ackType_ == AcknowledgementType::RAW) {
    socketCommunication_->send(data);
  }
  return ret;
}
} // namespace gramsballoon::pgrams