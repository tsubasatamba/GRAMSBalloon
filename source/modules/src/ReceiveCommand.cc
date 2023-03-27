#include "ReceiveCommand.hh"
#include <chrono>
#include <thread>

using namespace anlnext;

ReceiveCommand::ReceiveCommand()
  :baudrate_(B9600), openMode_(O_RDWR), readWaveformModuleName_("ReadWaveform")
{
  serialPath_ = "/dev/null";
  for (int i=0; i<2; i++) {
    que_.push(0);
  }
  comintp_ = std::make_unique<CommandInterpreter>(); 
}

ReceiveCommand::~ReceiveCommand() = default;

ANLStatus ReceiveCommand::mod_define()
{
  define_parameter("baudrate", &mod_class::baudrate_);
  define_parameter("serial_path", &mod_class::serialPath_);
  define_parameter("open_mode", &mod_class::openMode_);
  define_parameter("ReadWaveform_module_name", &mod_class::readWaveformModuleName_);

  return AS_OK;
}

ANLStatus ReceiveCommand::mod_initialize()
{
  if (exist_module(readWaveformModuleName_)) {
    get_module_NC(readWaveformModuleName_, &readWaveform_);
    DAQIO* io = readWaveform_->getDAQIO();
    comintp_->setDAQIO(io);
  }

  // communication
  sc_ = std::make_unique<SerialCommunication>(serialPath_, baudrate_, openMode_);
  sc_ -> initialize();

  return AS_OK;
}

ANLStatus ReceiveCommand::mod_analyze()
{
  uint8_t buffer = 0;
  const int status = sc_->sreadSingle(buffer);
  if (status == -1) {
    std::cerr << "Read command failed in ReceiveCommand::mod_analyze: status = " << status << std::endl;
    return AS_OK;
  }
  if (status == 0) {
    return AS_OK;
  }

  std::cout << "buffer: " << static_cast<int>(buffer) << std::endl;

  que_.push(buffer);
  que_.pop();
  if (startReading_) {
    command_.push_back(buffer);
    if (que_.front()==0xc5 && que_.back()==0xc5) {
      startReading_ = false;
      applyCommand();
    }
  }
  else {
    if (que_.front()==0xeb && que_.back()==0x90) {
      startReading_ = true;
      command_.clear();
      command_.push_back(que_.front());
      command_.push_back(que_.back());
    }
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(10));  
  
  
  return AS_OK;
}

ANLStatus ReceiveCommand::mod_finalize()
{
  return AS_OK;
}

void ReceiveCommand::applyCommand()
{
  comintp_ -> setCommand(command_);
  std::cout << "command start" << std::endl;
  for (int i=0; i<(int)command_.size(); i++) {
    std::cout << static_cast<int>(command_[i]) << std::endl;
  }
  if (!comintp_->isValid()) {
    std::cout << "Command is not valid consulting MD5 check." << std::endl;
    return;
  }

}
