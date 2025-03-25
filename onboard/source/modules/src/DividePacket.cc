#include "DividePacket.hh"
#include <iomanip>
using namespace anlnext;
namespace gramsballoon::pgrams {
ANLStatus DividePacket::mod_define() {
  define_parameter("ReceiveStatusFromDAQComputer_name", &mod_class::receiveStatusFromDAQComputerName_);
  set_parameter_description("Name of ReceiveStatusFromDAQComputer");
  define_parameter("chatter", &mod_class::chatter_);
  return AS_OK;
}
ANLStatus DividePacket::mod_initialize() {
  if (exist_module(receiveStatusFromDAQComputerName_)) {
    get_module_NC(receiveStatusFromDAQComputerName_, &receiveStatusFromDAQComputer_);
  }
  else {
    std::cerr << "ReceiveStatusFromDAQComputer module is not found." << std::endl;
    return AS_ERROR;
  }

  return AS_OK;
}
ANLStatus DividePacket::mod_analyze() {
  if (!receiveStatusFromDAQComputer_) {
    std::cerr << "ReceiveStatusFromDAQComputer is nullptr." << std::endl;
    return AS_OK;
  }
  if (receiveStatusFromDAQComputer_->GetBufferSize() == 0) {
    return AS_OK;
  }
  std::vector<uint8_t> *vec = nullptr;
  if (!inPacket_) {
    vec = &commandQueue_.emplace();
    inPacket_ = true;
  }
  else {
    vec = &commandQueue_.back();
  }
  uint16_t argc = 0;
  size_t packet_size = 0;
  for (int i = 0; i < MAX_BYTES; i++) {
    const auto byte = receiveStatusFromDAQComputer_->PopAndGetOneByte();
    const auto sz = vec->size();
    std::cout << "byte: " << std::hex << static_cast<int>(byte) << std::dec << std::endl;
    if (byte == 0xEB && sz == 0) {
      vec->push_back(byte);
    }
    else if (byte == 0x90 && sz == 1) {
      vec->push_back(byte);
    }
    else if (byte == 0x5B && sz == 2) {
      vec->push_back(byte);
    }
    else if (byte == 0x6A && sz == 3) {
      vec->push_back(byte);
    }
    else if (sz == 6) {
      argc = byte << 8;
      vec->push_back(byte);
    }
    else if (sz == 7) {
      argc |= byte;
      vec->push_back(byte);
      packet_size = 14 + 4 * argc;
      std::cout << "packet_size: " << static_cast<int>(packet_size) << std::endl;
    }
    else if ((sz > 4 && sz < packet_size - 4) || (sz == 4) || (sz == 5)) {
      vec->push_back(byte);
    }
    else if (sz == packet_size - 4 && byte == 0xC5) {
      vec->push_back(byte);
    }
    else if (sz == packet_size - 3 && byte == 0xA4) {
      vec->push_back(byte);
    }
    else if (sz == packet_size - 2 && byte == 0xD2) {
      vec->push_back(byte);
    }
    else if (sz == packet_size - 1 && byte == 0x79) {
      vec->push_back(byte);
      inPacket_ = false;
      if (chatter_ > 2) {
        std::cout << "Divided packet: ";
        for (const auto &byte: *vec) {
          std::cout << static_cast<int>(byte) << " ";
        }
        std::cout << std::endl;
      }
      break;
    }
    else {
      std::cerr << "DividePacket::mod_analyze: Error in packet division." << std::endl;
      vec->clear();
      inPacket_ = false;
      break;
    }
  }
  return AS_OK;
}
ANLStatus DividePacket::mod_finalize() {
  return AS_OK;
}
} // namespace gramsballoon::pgrams