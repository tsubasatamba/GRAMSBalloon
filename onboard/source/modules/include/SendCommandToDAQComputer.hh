#ifndef GRAMSBalloon_SendCommandToDAQComputer_hh
#define GRAMSBalloon_SendCommandToDAQComputer_hh 1
#include "DistributeCommand.hh"
#include "SendTelemetry.hh"
#include "SocketCommunicationManager.hh"
#include "anlnext/BasicModule.hh"
namespace gramsballoon::pgrams {
class SocketCommunicationManager;
class DistributeCommand;
class SendTelemetry;
class SendCommandToDAQComputer: public anlnext::BasicModule {
  DEFINE_ANL_MODULE(SendCommandToDAQComputer, 1.0);
  ENABLE_PARALLEL_RUN();

public:
  SendCommandToDAQComputer() = default;
  virtual ~SendCommandToDAQComputer() = default;

protected:
  SendCommandToDAQComputer(const SendCommandToDAQComputer &r) = default;

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_pre_initialize() override;
  anlnext::ANLStatus mod_begin_run() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_end_run() override;
  anlnext::ANLStatus mod_finalize() override;

private:
  SocketCommunicationManager *socketCommunicationManager_ = nullptr;
  std::string socketCommunicationManagerName_ = "SocketCommunicationManager";
  bool failed_ = false;
  int chatter_ = 0;
  DistributeCommand *distributeCommand_ = nullptr;
  std::string distributeCommandName_ = "DistributeCommand";
  SendTelemetry *sendTelemetry_ = nullptr;
};
} // namespace gramsballoon::pgrams
#endif //GRAMSBalloon_SendCommandToDAQComputer_hh