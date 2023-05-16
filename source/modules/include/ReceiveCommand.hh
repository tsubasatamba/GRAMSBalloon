/**
 * Receive commands from ground.
 *
 * @author Tsubasa Tamba, Shota Arai
 * @date 2023-03-01
 */


#ifndef ReceiveCommand_H
#define ReceiveCommand_H 1

#include <anlnext/BasicModule.hh>
#include <queue>
#include <sys/select.h>
#include <sys/time.h>
#include "CommandDefinition.hh"
#include "SerialCommunication.hh"
#include "ShutdownSystem.hh"
#include "SendTelemetry.hh"
#include "ReadWaveform.hh"
#include "ControlHighVoltage.hh"


namespace gramsballoon {

class ShutdownSystem;
class SendTelemetry;
class ReadWaveform;
class CotrolHighVoltage;

class ReceiveCommand : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(ReceiveCommand, 1.0);
  ENABLE_PARALLEL_RUN();

public:
  ReceiveCommand();
  virtual ~ReceiveCommand();
protected:
  ReceiveCommand(const ReceiveCommand& r) = default;

public:  
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_finalize() override;

  bool applyCommand();

  uint16_t CommandCode() { return (singleton_self()->comdef_)->Code(); }
  uint32_t CommandIndex() { return singleton_self()->commandIndex_; }
  uint16_t CommandRejectCount() { return singleton_self()->commandRejectCount_; }

private:
  std::vector<uint8_t> buffer_;
  std::vector<uint8_t> command_;
  std::shared_ptr<CommandDefinition> comdef_ = nullptr;
  uint32_t commandIndex_ = 0;
  uint16_t commandRejectCount_ = 0;

  // access to other classes
  SendTelemetry* sendTelemetry_ = nullptr;
  ShutdownSystem* shutdownSystem_ = nullptr;
  ReadWaveform* readWaveform_ = nullptr;
  ControlHighVoltage* TPCHVController_ = nullptr;
  std::string TPCHVControllerModuleName_ = "";
  ControlHighVoltage* PMTHVController_ = nullptr;
  std::string PMTHVControllerModuleName_ = "";

  //communication
  std::shared_ptr<SerialCommunication> sc_ = nullptr;
  speed_t baudrate_;
  std::string serialPath_;
  mode_t openMode_ = O_RDWR;
  bool startReading_ = false;
  int timeoutSec_ = 10;
};

} /* namespace gramsballoon */

#endif /* ReceiveCommand_H */
