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
#include "ReadWaveform.hh"
#include "SerialCommunication.hh"

class ReadWaveform;

namespace GRAMSBalloon {

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

  void applyCommand();

  uint16_t LastCommandCode() { return comdef_->Code(); }

private:
  std::vector<uint8_t> command_;
  std::queue<uint8_t> que_;
  std::shared_ptr<CommandDefinition> comdef_ = nullptr;
  ReadWaveform* readWaveform_ = nullptr;
  std::string readWaveformModuleName_ = "ReadWaveform";

  //communication
  std::shared_ptr<SerialCommunication> sc_ = nullptr;
  speed_t baudrate_;
  std::string serialPath_;
  char openMode_ = O_RDWR;
  bool startReading_ = false;
  
};

} /* namespace GRAMSBalloon */

#endif /* ReceiveCommand_H */
