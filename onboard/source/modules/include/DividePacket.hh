#ifndef DivideCommand_hh
#define DivideCommand_hh 1
#include "ReceiveStatusFromDAQComputer.hh"
#include "anlnext/BasicModule.hh"
#include <queue>
#include <vector>
namespace gramsballoon::pgrams {
class ReceiveStatusFromDAQComputer;
class DividePacket: public anlnext::BasicModule {
  DEFINE_ANL_MODULE(DividePacket, 1.0);
  ENABLE_PARALLEL_RUN();

public:
  DividePacket() = default;
  virtual ~DividePacket() = default;

protected:
  DividePacket(const DividePacket &r) = default;

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_finalize() override;
  bool IsEmpty() const {
    return commandQueue_.empty();
  }
  const std::vector<uint8_t> &GetLastPacket() const {
    return commandQueue_.front();
  }
  void PopPacket() {
    if (!commandQueue_.empty())
      commandQueue_.pop();
  }

private:
  ReceiveStatusFromDAQComputer *receiveStatusFromDAQComputer_ = nullptr;
  std::string receiveStatusFromDAQComputerName_ = "ReceiveStatusFromDAQComputer";
  std::queue<std::vector<uint8_t>> commandQueue_;
  std::vector<uint8_t> vec;
  int chatter_ = 0;
  size_t lastPacketSize_ = 0;
  static constexpr int MAX_BYTES = 1024;
};
} // namespace gramsballoon::pgrams
#endif // DivideCommand_hh