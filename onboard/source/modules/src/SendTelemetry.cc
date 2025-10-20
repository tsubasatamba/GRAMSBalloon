#include "SendTelemetry.hh"

using namespace anlnext;

namespace gramsballoon::pgrams {

SendTelemetry::SendTelemetry() {
  errorManager_ = std::make_shared<ErrorManager>();
  binaryFilenameBase_ = "Telemetry";
}

SendTelemetry::~SendTelemetry() = default;

ANLStatus SendTelemetry::mod_define() {
  define_parameter("save_telemetry", &mod_class::saveTelemetry_);
  define_parameter("binary_filename_base", &mod_class::binaryFilenameBase_);
  define_parameter("num_telem_per_file", &mod_class::numTelemPerFile_);
  define_parameter("sleep_for_msec", &mod_class::sleepms_);
  define_parameter("topics", &mod_class::pubTopics_);
  define_parameter("qos", &mod_class::qos_);
  define_parameter("chatter", &mod_class::chatter_);
  return AS_OK;
}

ANLStatus SendTelemetry::mod_initialize() {

  const std::string receive_command_md = "ReceiveCommand";
  if (exist_module(receive_command_md)) {
    get_module_NC(receive_command_md, &receiveCommand_);
  }

  const std::string run_id_manager_md = "RunIDManager";
  if (exist_module(run_id_manager_md)) {
    get_module_NC(run_id_manager_md, &runIDManager_);
  }

  // communication
  if (exist_module("TelemMosquittoManager")) {
    get_module_NC("TelemMosquittoManager", &mosquittoManager_);
  }
  else {
    std::cerr << "Error in SendTelemetry::mod_initialize: MosquittoManager module not found." << std::endl;
    return AS_ERROR;
  }
  mosq_ = mosquittoManager_->getMosquittoIO();
  if (!mosq_) {
    std::cerr << "MosquittoIO is nullptr" << std::endl;
    return AS_ERROR;
  }
  telemdef_ = std::make_shared<HubHKTelemetry>(true);
  if (saveTelemetry_) {
    telemetrySaver_ = std::make_shared<CommunicationSaver<std::string>>();
  }
  if (telemetrySaver_) {
    telemetrySaver_->setBinaryFilenameBase(binaryFilenameBase_);
    if (runIDManager_) {
      telemetrySaver_->setRunID(runIDManager_->RunID());
      telemetrySaver_->setTimeStampStr(runIDManager_->TimeStampStr());
    }
  }
  return AS_OK;
}

ANLStatus SendTelemetry::mod_analyze() {
  if (chatter_ > 0) std::cout << "SendTelemetry::mod_analyze" << std::endl;
  telemetryStr_.clear();
  if (mosq_ == nullptr) {
    std::cout << module_id() << ": mosq_ is nullptr" << std::endl;
    return AS_OK;
  }
  telemdef_->setCurrentTime();
  telemdef_->setIndex(telemIndex_);
  for (int i = 0; i < static_cast<int>(HubHKTelemetry::NUM_ERROR_FLAGS); i++) {
    telemdef_->setHubComputerErrorFlags(i, errorManager_->ErrorCode(i));
  }
  telemdef_->setRunID(runIDManager_->RunID());
  telemIndex_++;
  telemdef_->update();
  telemdef_->construct(telemetryStr_);
  std::cout << module_id() << ":telelemetry is " << telemetryStr_ << std::endl;
  for (const auto &topic: pubTopics_) {
    const int status = mosq_->Publish(telemetryStr_, topic, qos_);
    const bool failed = (status != mosq_err_t::MOSQ_ERR_SUCCESS);
    if (failed) {
      std::cerr << "Error in SendTelemetry::mod_analyze: Publishing MQTT failed.Error Message: " << mosqpp::strerror(status) << std::endl;
      errorManager_->setError(ErrorType::MQTT_COM_ERROR);
    }
    if (saveTelemetry_ && telemetrySaver_) {
      telemetrySaver_->writeCommandToFile(failed, telemetryStr_);
    }
  }

  if (chatter_ >= 1) {
    std::cout << (int)telemetryStr_.size() << std::endl;
    for (int i = 0; i < (int)telemetryStr_.size(); i++) {
      std::cout << i << " " << static_cast<int>(telemetryStr_[i]) << std::endl;
    }
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(sleepms_));
  return AS_OK;
}

ANLStatus SendTelemetry::mod_finalize() {
  return AS_OK;
}

} // namespace gramsballoon::pgrams
