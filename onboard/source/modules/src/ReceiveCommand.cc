#include "ReceiveCommand.hh"
#include <chrono>
#include <thread>

using namespace anlnext;
using namespace gramsballoon::pgrams;
namespace gramsballoon {

ReceiveCommand::ReceiveCommand() {
  binaryFilenameBase_ = "Command";
  topic_ = "command";
  comdef_ = std::make_shared<CommunicationFormat>();
}

ReceiveCommand::~ReceiveCommand() = default;

ANLStatus ReceiveCommand::mod_define() {
  define_parameter("timeout_sec", &mod_class::timeoutSec_);
  define_parameter("save_command", &mod_class::saveCommand_);
  define_parameter("binary_filename_base", &mod_class::binaryFilenameBase_);
  define_parameter("num_command_per_file", &mod_class::numCommandPerFile_);
  define_parameter("topic", &mod_class::topic_);
  define_parameter("qos", &mod_class::qos_);
  define_parameter("chatter", &mod_class::chatter_);
  return AS_OK;
}

ANLStatus ReceiveCommand::mod_initialize() {
  const std::string send_telem_md = "SendTelemetry";
  if (exist_module(send_telem_md)) {
    get_module_NC(send_telem_md, &sendTelemetry_);
  }

  const std::string shutdown_system_md = "ShutdownSystem";
  if (exist_module(shutdown_system_md)) {
    get_module_NC(shutdown_system_md, &shutdownSystem_);
  }



  const std::string run_id_manager_md = "RunIDManager";
  if (exist_module(run_id_manager_md)) {
    get_module_NC(run_id_manager_md, &runIDManager_);
  }

  const std::string mosq_md = "MosquittoManager";
  if (exist_module(mosq_md)) {
    get_module_NC(mosq_md, &mosquittoManager_);
  }
  else {
    std::cerr << "Error in ReceiveCommand::mod_initialize: MosquittoManager module not found." << std::endl;
    if (sendTelemetry_) {
      sendTelemetry_->getErrorManager()->setError(ErrorType::MODULE_ACCESS_ERROR);
      return AS_ERROR;
    }
  }
  // communication
  mosq_ = mosquittoManager_->getMosquittoIO();
  if (!mosq_) {
    std::cerr << "MosquittoIO is nullptr" << std::endl;
    return AS_ERROR;
  }
  const int sub_result = mosq_->subscribe(NULL, topic_.c_str(), qos_);
  if (sub_result != 0) {
    std::cerr << "Error in ReceiveCommand::mod_initialize: Subscribing MQTT failed. Error Message: " << mosqpp::strerror(sub_result) << std::endl;
    if (sendTelemetry_) {
      sendTelemetry_->getErrorManager()->setError(ErrorType::RECEIVE_COMMAND_SERIAL_COMMUNICATION_ERROR);
    }
  }
  return AS_OK;
}

ANLStatus ReceiveCommand::mod_analyze() {
  if (!mosq_) {
    return AS_OK;
  }
  auto commands = mosq_->getPayload();
  if (commands.empty()) {
    return AS_OK;
  }
  const size_t sz = commands.size();
  if (chatter_ >= 1) {
    std::cout << "ReceiveCommand Num_packet:" << sz << std::endl;
  }
  if (chatter_ >= 2) {
    for (int j = 0; j < static_cast<int>(commands[0]->payload.size()); j++) {
      std::cout << "ReceiveCommand Payload[" << 0 << "][" << j << "]:" << static_cast<int>(commands[0]->payload[j]) << std::endl;
    }
  }
  const auto command = commands[0];
  const auto &command_payload = command->payload;
  if (command->topic != topic_) {
    return AS_OK;
  }
  const bool applied = applyCommand(command_payload);
  writeCommandToFile(!applied, command_payload);
  if (!applied) {
    commandRejectCount_++;
    if (sendTelemetry_) {
      sendTelemetry_->getErrorManager()->setError(ErrorType::INVALID_COMMAND);
    }
  }
  mosq_->popPayloadFront();
  return AS_OK;
}

ANLStatus ReceiveCommand::mod_finalize() {
  return AS_OK;
}

bool ReceiveCommand::applyCommand(const std::vector<uint8_t> &command) {
  commandIndex_++;
  if (chatter_ >= 1) {
    std::cout << "command start" << std::endl;
    std::cout << "command index: " << commandIndex_ << std::endl;
  }
  if (chatter_ >= 2) {
    for (int i = 0; i < (int)command.size(); i++) {
      std::cout << static_cast<int>(command[i]) << std::endl;
    }
  }
  const bool status = comdef_->setData(command);
  if (!status) {
    return false;
  }
  comdef_->interpret();

  const uint16_t code = comdef_->Code();
  const uint16_t argc = comdef_->Argc();
  const std::vector<int32_t> arguments = comdef_->Arguments();
  if (chatter_ >= 1) {
    std::cout << "code: " << code << std::endl;
    std::cout << "argc: " << argc << std::endl;
    for (int i = 0; i < argc; i++) {
      std::cout << "arguments[" << i << "]: " << arguments[i] << std::endl;
    }
  }

  if (code == 900 && argc == 0) {
    return true;
  }

  if (code == 901 && argc == 1) {
    return true;
  }

  if (code == 902 && argc == 0) {
    return true;
  }

  return false;
}

void ReceiveCommand::writeCommandToFile(bool failed, const std::vector<uint8_t> &command) {
  int type = 1;
  std::string type_str = "";
  if (failed) {
    type = 0;
  }
  if (type == 1) type_str = "normal";
  if (type == 0) type_str = "failed";

  const bool app = true;
  if (fileIDmp_.find(type) == fileIDmp_.end()) {
    fileIDmp_[type] = std::pair<int, int>(0, 0);
  }
  else if (fileIDmp_[type].second == numCommandPerFile_) {
    fileIDmp_[type].first++;
    fileIDmp_[type].second = 0;
  }

  int run_id = 0;
  std::string time_stamp_str = "YYYYMMDDHHMMSS";
  if (runIDManager_) {
    run_id = runIDManager_->RunID();
    time_stamp_str = runIDManager_->TimeStampStr();
  }
  std::ostringstream id_sout;
  id_sout << std::setfill('0') << std::right << std::setw(6) << fileIDmp_[type].first;
  const std::string id_str = id_sout.str();
  std::ostringstream run_id_sout;
  run_id_sout << std::setfill('0') << std::right << std::setw(6) << run_id;
  const std::string run_id_str = run_id_sout.str();
  const std::string filename = binaryFilenameBase_ + "_" + run_id_str + "_" + time_stamp_str + "_" + type_str + "_" + id_str + ".dat";

  if (!failed) {
    comdef_->writeFile(filename, app);
  }
  else {
    writeVectorToBinaryFile(filename, app, command);
  }
  fileIDmp_[type].second++;
}

} /* namespace gramsballoon */
