#include "ReceiveCommand.hh"
#include <chrono>
#include <thread>

using namespace anlnext;

namespace gramsballoon {

ReceiveCommand::ReceiveCommand()
  : baudrate_(B9600), openMode_(O_RDWR)
{
  serialPath_ = "/dev/null";
  comdef_ = std::make_shared<CommandDefinition>(); 
}

ReceiveCommand::~ReceiveCommand() = default;

ANLStatus ReceiveCommand::mod_define()
{
  define_parameter("baudrate", &mod_class::baudrate_);
  define_parameter("serial_path", &mod_class::serialPath_);
  define_parameter("open_mode", &mod_class::openMode_);

  return AS_OK;
}

ANLStatus ReceiveCommand::mod_initialize()
{
  const std::string send_telem_md = "SendTelemetry";
  if (exist_module(send_telem_md)) {
    get_module_NC(send_telem_md, &sendTelemetry_);
  }

  const std::string shutdown_system_md = "ShutdownSystem";
  if (exist_module(shutdown_system_md)) {
    get_module_NC(shutdown_system_md, &shutdownSystem_);
  }

  const std::string read_wf_md = "ReadWaveform";
  if (exist_module(read_wf_md)) {
    get_module_NC(read_wf_md, &readWaveform_);
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

  #if 1
  for (int i=0; i<status; i++) {
    std::cout << "command[" << i << "] = " << static_cast<int>(command_[i]) << std::endl;
  }
  #endif

  const bool applied = applyCommand();
  if (!applied) {
    commandRejectCount_++;
  }
  
  return AS_OK;
}

ANLStatus ReceiveCommand::mod_finalize()
{
  return AS_OK;
}

bool ReceiveCommand::applyCommand()
{
  commandIndex_++;
  std::cout << "command start" << std::endl;
  for (int i=0; i<(int)command_.size(); i++) {
    std::cout << static_cast<int>(command_[i]) << std::endl;
  }
  bool status = comdef_ -> setCommand(command_);
  if (!status) {
    return false;
  }
  comdef_->interpret();
  
  const int code = comdef_->Code();

  if (code==100) {
    if (sendTelemetry_!=nullptr) {
      sendTelemetry_->setTelemetryType(3);
      return true;
    }
    else {
      return false;
    }
  }

  if (code==101) {}

  if (code==102) {
    if (shutdownSystem_!=nullptr) {
      shutdownSystem_->setShutdown(true);
      return true;
    }
    else {
      return false;
    }
  }

  if (code==103) {
    if (shutdownSystem_!=nullptr) {
      shutdownSystem_->setReboot(true);
      return true;
    }
    else {
      return false;
    }
  }

  if (code==104) {
    if (shutdownSystem_!=nullptr) {
      shutdownSystem_->setPrepareShutdown(true);
    }
    else {
      return false;
    }
  }

  if (code==105) {
    if (shutdownSystem_!=nullptr) {
      shutdownSystem_->setPrepareReboot(true);
      return true;
    }
    else {
      return false;
    }
  }

  if (code==201) {}

  if (code==210) {
    if (readWaveform_!=nullptr) {
      readWaveform_->setOndemand(true);
      return true;
    }
    else {
      return false;
    }
  }

  return false;
}

} /* namespace gramsballoon */
