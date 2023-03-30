#include "ReceiveCommand.hh"
#include <chrono>
#include <thread>

using namespace anlnext;

namespace gramsballoon {

ReceiveCommand::ReceiveCommand()
  :readWaveformModuleName_("ReadWaveform"), baudrate_(B9600), openMode_(O_RDWR)
{
  serialPath_ = "/dev/null";
  for (int i=0; i<2; i++) {
    que_.push(0);
  }
  comdef_ = std::make_shared<CommandDefinition>(); 
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
  }

  // communication
  sc_ = std::make_shared<SerialCommunication>(serialPath_, baudrate_, openMode_);
  sc_ -> initialize();

  

  return AS_OK;
}

ANLStatus ReceiveCommand::mod_analyze()
{
  std::this_thread::sleep_for(std::chrono::seconds(1));
  const int max_trial = 26;

  command_.clear();

  fd_set fdset;
  timeval timeout;
  timeout.tv_sec = 10;
  timeout.tv_usec = 0;
  FD_ZERO(&fdset);
  FD_SET(sc_->FD(), &fdset);
  int rv = select(sc_->FD() + 1, &fdset, NULL, NULL, &timeout);

  std::cout << "rv: " << rv << std::endl;

  if (rv == -1) {
    std::cerr << "Error in ReceiveCommand::mod_analyze: rv = -1" << std::endl;
    return AS_ERROR;
  }
  
  if (rv==0) {
    std::cout << "Time out" << std::endl;
    return AS_OK;
  }

  std::vector<uint8_t> buffer(max_trial);
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  const int status = sc_->sread(buffer, max_trial);
  if (status == -1) {
    std::cerr << "Read command failed in ReceiveCommand::mod_analyze: status = " << status << std::endl;
    return AS_OK;
  }

  std::cout << "status: " << status << std::endl;
  for (int i=0; i<status; i++) {
    command_.push_back(buffer[i]);
  }
  for (int i=0; i<status; i++) {
    std::cout << "command[" << i << "] = " << static_cast<int>(command_[i]) << std::endl;
  }
  applyCommand();                                    
  
/*
  for (int i=0; i<status; i++) {
    que_.push(buffer[i]);
    que_.pop();
    if (startReading_) {
      command_.push_back(buffer[i]);
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
  }
*/
  
  
  return AS_OK;
}

ANLStatus ReceiveCommand::mod_finalize()
{
  return AS_OK;
}

void ReceiveCommand::applyCommand()
{
  comdef_ -> setCommand(command_);
  std::cout << "command start" << std::endl;
  for (int i=0; i<(int)command_.size(); i++) {
    std::cout << static_cast<int>(command_[i]) << std::endl;
  }
  bool status = comdef_->interpret();
  if (!status) {
    return;
  }

  const int code = comdef_->Code();

  if (code==210) {
    readWaveform_->setOndemand(true);
  }

}

} /* namespace gramsballoon */
