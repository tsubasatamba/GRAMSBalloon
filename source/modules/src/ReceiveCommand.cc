#include "ReceiveCommand.hh"
#include <chrono>
#include <thread>

using namespace anlnext;

namespace gramsballoon {

ReceiveCommand::ReceiveCommand()
  : baudrate_(B9600), openMode_(O_RDWR)
{
  TPCHVControllerModuleName_ = "ControlHighVoltage_TPC";
  PMTHVControllerModuleName_ = "ControlHighVoltage_PMT";
  serialPath_ = "/dev/null";
  binaryFilenameBase_ = "Command";
  comdef_ = std::make_shared<CommandDefinition>(); 
  buffer_.resize(200);
}

ReceiveCommand::~ReceiveCommand() = default;

ANLStatus ReceiveCommand::mod_define()
{
  define_parameter("baudrate", &mod_class::baudrate_);
  define_parameter("serial_path", &mod_class::serialPath_);
  define_parameter("open_mode", &mod_class::openMode_);
  define_parameter("TPC_HVController_module_name", &mod_class::TPCHVControllerModuleName_);
  define_parameter("PMT_HVController_module_name", &mod_class::PMTHVControllerModuleName_);
  define_parameter("timeout_sec", &mod_class::timeoutSec_);
  define_parameter("save_command", &mod_class::saveCommand_);
  define_parameter("binary_filename_base", &mod_class::binaryFilenameBase_);
  define_parameter("num_command_per_file", &mod_class::numCommandPerFile_);
  define_parameter("chatter", &mod_class::chatter_);

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

  if (exist_module(TPCHVControllerModuleName_)) {
    get_module_NC(TPCHVControllerModuleName_, &TPCHVController_);
  }

  if (exist_module(PMTHVControllerModuleName_)) {
    get_module_NC(PMTHVControllerModuleName_, &PMTHVController_);
  }

  // communication
  sc_ = std::make_shared<SerialCommunication>(serialPath_, baudrate_, openMode_);
  const int status = sc_ -> initialize();
  if (status!=0) {
    std::cerr << "Error in ReceiveCommand::mod_initialize: Serial communication failed." << std::endl;
    if (sendTelemetry_) {
      sendTelemetry_->getErrorManager()->setError(ErrorType::RECEIVE_COMMAND_SERIAL_COMMUNICATION_ERROR);
    }
  }

  return AS_OK;
}

ANLStatus ReceiveCommand::mod_analyze()
{
  fd_set fdset;
  timeval timeout;
  timeout.tv_sec = timeoutSec_;
  timeout.tv_usec = 0;
  FD_ZERO(&fdset);
  FD_SET(sc_->FD(), &fdset);
  int rv = select(sc_->FD() + 1, &fdset, NULL, NULL, &timeout);
  if (rv == -1) {
    std::cerr << "Error in ReceiveCommand::mod_analyze: rv = -1" << std::endl;
    if (sendTelemetry_) {
      sendTelemetry_->getErrorManager()->setError(ErrorType::RECEIVE_COMMAND_SELECT_ERROR);
    }
    return AS_OK;
  }
  
  if (rv==0) {
    std::cout << "Time out" << std::endl;
    return AS_OK;
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  const int status = sc_->sread(buffer_, 200);
  if (status == -1) {
    std::cerr << "Read command failed in ReceiveCommand::mod_analyze: status = " << status << std::endl;
    if (sendTelemetry_) {
      sendTelemetry_->getErrorManager()->setError(ErrorType::RECEIVE_COMMAND_SREAD_ERROR);
    }
    return AS_OK;
  }

  for (int i=0; i<status; i++) {
    if (i<status-1 && buffer_[i]==0xeb && buffer_[i+1]==0x90) {
      command_.clear();
    }
    command_.push_back(buffer_[i]);
    if (i>0 && buffer_[i-1]==0xc5 && buffer_[i]==0xc5) {
      break;
    }
  }

  if (chatter_>=1) {
    std::cout << "ReceiveCommand status: " << status << std::endl;
    for (int i=0; i<status; i++) {
      std::cout << "command[" << i << "] = " << static_cast<int>(command_[i]) << std::endl;
    }
  }

  const bool applied = applyCommand();
  writeCommandToFile(!applied);
  if (!applied) {
    if (sendTelemetry_) {
      sendTelemetry_->getErrorManager()->setError(ErrorType::INVALID_COMMAND);
    }
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
  std::cout << "command index: " << commandIndex_ << std::endl;
  for (int i=0; i<(int)command_.size(); i++) {
    std::cout << static_cast<int>(command_[i]) << std::endl;
  }
  bool status = comdef_ -> setCommand(command_);
  if (!status) {
    return false;
  }
  comdef_->interpret();
  
  const uint16_t code = comdef_->Code();
  const uint16_t argc = comdef_->Argc();
  const std::vector<int32_t> arguments = comdef_->Arguments();

  if (code==100 && argc==0) {
    if (sendTelemetry_!=nullptr) {
      sendTelemetry_->setTelemetryType(static_cast<int>(TelemetryType::Status));
      return true;
    }
  }

  if (code==101 && argc==0) {
    if (sendTelemetry_!=nullptr) {
      sendTelemetry_->getErrorManager()->resetError();
      return true;
    }
  }

  if (code==102 && argc==0) {
    if (shutdownSystem_!=nullptr) {
      shutdownSystem_->setShutdown(true);
      return true;
    }
  }

  if (code==103 && argc==0) {
    if (shutdownSystem_!=nullptr) {
      shutdownSystem_->setReboot(true);
      return true;
    }
  }

  if (code==104 && argc==0) {
    if (shutdownSystem_!=nullptr) {
      shutdownSystem_->setPrepareShutdown(true);
      return true;
    }
  }

  if (code==105 && argc==0) {
    if (shutdownSystem_!=nullptr) {
      shutdownSystem_->setPrepareReboot(true);
      return true;
    }
  }

  if (code==201 && argc==0) {
    if (readWaveform_!=nullptr) {
      readWaveform_->setStartReading(true);
      return true;
    }
  }

  if (code==202 && argc==0) {
    if (readWaveform_!=nullptr) {
      readWaveform_->setStartReading(false);
      return true;
    }
  }

  if (code==203 && argc==1) {
    if (readWaveform_!=nullptr) {
      readWaveform_->setTrigMode(static_cast<int>(arguments[0]));
      return true;
    }
  }

  if (code==204 && argc==2) {
    if (readWaveform_!=nullptr) {
      readWaveform_->setTrigDevice(static_cast<int>(arguments[0]));
      readWaveform_->setTrigChannel(static_cast<int>(arguments[1]));
      return true;
    }
  }

  if (code==205 && argc==3) {
    if (readWaveform_!=nullptr) {
      const int device = static_cast<int>(arguments[0]);
      const int channel = static_cast<int>(arguments[1]);
      const double v = static_cast<double>(arguments[2]) * 1E-3;
      const int index = device*2 + channel;
      readWaveform_->setADCOffset(index, v);
      return true;
    }
  }

  if (code==206 && argc==0) {
    if (TPCHVController_!=nullptr) {
      TPCHVController_->setExec(true);
      return true;
    }
  }

  if (code==207 && argc==1) {
    if (TPCHVController_!=nullptr) {
      const double v = static_cast<double>(arguments[0]) * 1E-3;
      TPCHVController_->setNextVoltage(v);
      return true;
    }
  }

  if (code==208 && argc==0) {
    if (PMTHVController_!=nullptr) {
      PMTHVController_->setExec(true);
      return true;
    }
  }

  if (code==209 && argc==1) {
    if (PMTHVController_!=nullptr) {
      const double v = static_cast<double>(arguments[0]) * 1E-3;
      PMTHVController_->setNextVoltage(v);
      return true;
    }
  }

  if (code==210 && argc==0) {
    if (readWaveform_!=nullptr) {
      readWaveform_->setOndemand(true);
      return true;
    }
  }

  if (code==211 && argc==1) {
    if (readWaveform_!=nullptr) {
      const double v = static_cast<double>(arguments[0]) * 1E-3;
      readWaveform_->setTrigLevel(v);
      return true;
    }
  }

  if (code==212 && argc==1) {
    if (readWaveform_!=nullptr) {
      const double v = static_cast<double>(arguments[0]) * 1E-3;
      readWaveform_->setTrigPosition(v);
      return true;
    }
  }

  if (code==900 && argc==0) {
    return true;
  }

  if (code==901 && argc==1) {
    return true;
  }

  if (code==902 && argc==0) {
    return true;
  }

  return false;
}

void ReceiveCommand::writeCommandToFile(bool failed)
{
  int type = 1;
  std::string type_str = "";
  if (failed) {
    type = 0;
  }
  if (type==1) type_str = "normal";
  if (type==0) type_str = "failed";

  const bool app = true;
  if (fileIDmp_.find(type)==fileIDmp_.end()) {
    fileIDmp_[type] = std::pair<int, int>(0, 0);
  }
  else if (fileIDmp_[type].second==numCommandPerFile_) {
    fileIDmp_[type].first++;
    fileIDmp_[type].second = 0;
  }

  std::ostringstream sout;
  sout << std::setfill('0') << std::right << std::setw(6) << fileIDmp_[type].first;
  const std::string id_str = sout.str();
  const std::string filename = binaryFilenameBase_ + "_" + timeStampStr_ + "_" + type_str + "_" + id_str + ".dat";
  
  if (!failed) {
    comdef_->writeFile(filename, app);
  }
  else {
    writeVectorToBinaryFile(filename, app, command_);
  }
  fileIDmp_[type].second++;
}


} /* namespace gramsballoon */
