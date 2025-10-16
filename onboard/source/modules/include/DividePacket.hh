#ifndef DivideCommand_hh
#define DivideCommand_hh 1
#include "CommunicationFormat.hh"
#include "IridiumPacketPool.hh"
#include "ReceiveStatusFromDAQComputer.hh"
#include "anlnext/BasicModule.hh"
#include <queue>
#include <vector>
namespace gramsballoon::pgrams {
class ReceiveStatusFromDAQComputer;
class IridiumPacketPool;
class DividePacket: public anlnext::BasicModule {
  DEFINE_ANL_MODULE(DividePacket, 1.0);
  ENABLE_PARALLEL_RUN();

public:
  DividePacket();
  virtual ~DividePacket() = default;

protected:
  DividePacket(const DividePacket &r) = default;

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_finalize() override;
  bool IsEmptyIridium() const {
    return iridiumPacketQueue_->empty();
  }
  const std::shared_ptr<CommunicationFormat> GetLastPacketIridium() const {
    return iridiumPacketQueue_->front();
  }
  void PopPacketIridium() {
    if (!iridiumPacketQueue_->empty())
      iridiumPacketQueue_->pop();
  }
  bool IsEmptyStarlink() const {
    return starlinkPacketQueue_.empty();
  }
  const std::shared_ptr<CommunicationFormat> GetLastPacketStarlink() const {
    return starlinkPacketQueue_.front();
  }
  void PopPacketStarlink() {
    if (!starlinkPacketQueue_.empty())
      starlinkPacketQueue_.pop();
  }
  void PopPacket(bool isStarlink) {
    if (isStarlink) {
      PopPacketStarlink();
    }
    else {
      PopPacketIridium();
    }
  }
  bool IsEmpty(bool isStarlink) const {
    if (isStarlink) {
      return IsEmptyStarlink();
    }
    else {
      return IsEmptyIridium();
    }
  }
  const std::shared_ptr<CommunicationFormat> GetLastPacket(bool isStarlink) const {
    if (isStarlink) {
      return GetLastPacketStarlink();
    }
    else {
      return GetLastPacketIridium();
    }
  }

private:
  void PushCurrentVector();
  ReceiveStatusFromDAQComputer *receiveStatusFromDAQComputer_ = nullptr;
  std::string receiveStatusFromDAQComputerName_ = "ReceiveStatusFromDAQComputer";
  std::vector<int> starlinkCode_;
  std::shared_ptr<IridiumPacketPool> iridiumPacketQueue_;
  uint16_t overwrittenPacketCode_ = UINT16_MAX;
  std::queue<std::shared_ptr<CommunicationFormat>> starlinkPacketQueue_;
  std::vector<uint8_t> currentPacket_;
  int chatter_ = 0;
  size_t lastPacketSize_ = 0;
  uint16_t currentCode_ = 0;
  bool inError_ = false;
  static constexpr int MAX_BYTES = 1024;
};
} // namespace gramsballoon::pgrams
#endif // DivideCommand_hh