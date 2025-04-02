/**
 * Module for receiving from DAQ computers.
 *
 * @author Shota Arai
 * @date 2025-03-24 | First design
 *
 */
#ifndef GB_ReceiveStatusFromDAQComputer_hh
#define GB_ReceiveStatusFromDAQComputer_hh 1
#include "CommunicationFormat.hh"
#include "SendTelemetry.hh"
#include "SocketCommunicationManager.hh"
#include "anlnext/BasicModule.hh"
#include <memory>
#include <queue>
#include <string>
namespace gramsballoon {
class SendTelemetry;
}
namespace gramsballoon::pgrams {
class SocketCommunicationManager;
class ReceiveStatusFromDAQComputer: public anlnext::BasicModule {
  DEFINE_ANL_MODULE(ReceiveStatusFromDAQComputer, 1.0);
  ENABLE_PARALLEL_RUN();

public:
  ReceiveStatusFromDAQComputer() = default;
  virtual ~ReceiveStatusFromDAQComputer() = default;

protected:
  ReceiveStatusFromDAQComputer(const ReceiveStatusFromDAQComputer &r) = default;

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_finalize() override;
  uint8_t PopAndGetOneByte() {
    const uint8_t byte = buffer_.front();
    buffer_.pop();
    return byte;
  }
  size_t GetBufferSize() {
    return buffer_.size();
  }

private:
  SocketCommunicationManager *socketCommunicationManager_ = nullptr;
  SendTelemetry *sendTelemetry_ = nullptr;
  std::string socketCommunicationManagerName_ = "SocketCommunicationManager";
  std::shared_ptr<CommunicationFormat> commandDefinition_ = nullptr;
  int chatter_ = 0;
  std::queue<uint8_t> buffer_;
  static constexpr int MAX_BYTES = 1024;
};
} // namespace gramsballoon::pgrams
#endif // GB_ReceiveStatusFromDAQComputer_hh