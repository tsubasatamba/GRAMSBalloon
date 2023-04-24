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
#include "SendTelemetry.hh"
#include "ShutdownSystem.hh"
#include "ReadWaveform.hh"


namespace gramsballoon {

class SendTelemetry;
class ShutdownSystem;
class ReadWaveform;

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

  uint16_t CommandCode() { return comdef_->Code(); }
  uint32_t CommandIndex() { return commandIndex_; }
  uint16_t CommandRejectCount() { return commandRejectCount_; }

private:
  std::vector<uint8_t> command_;
  std::shared_ptr<CommandDefinition> comdef_ = nullptr;
  uint32_t commandIndex_ = 0;
  uint16_t commandRejectCount_ = 0;

  // access to other classes
  SendTelemetry* sendTelemetry_ = nullptr;
  ShutdownSystem* shutdownSystem_ = nullptr;
  ReadWaveform* readWaveform_ = nullptr;

  //communication
  std::shared_ptr<SerialCommunication> sc_ = nullptr;
  speed_t baudrate_;
  std::string serialPath_;
  mode_t openMode_ = O_RDWR;
  bool startReading_ = false;
  
};

} /* namespace gramsballoon */

#endif /* ReceiveCommand_H */
