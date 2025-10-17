#include "InterpretTelemetry.hh"
#include "DateManager.hh"
#include "HubHKTelemetry.hh"
#include <sstream>
using namespace anlnext;

namespace gramsballoon::pgrams {

template <typename TelemType>
InterpretTelemetry<TelemType>::InterpretTelemetry() {
  errorManager_ = std::make_shared<ErrorManager>();
  binaryFilenameBase_ = "Telemetry";
  runIDFilename_ = "/Users/grams/settings/run_id/run_id.txt";
  receiverModuleName_ = "ReceiveTelemetry";
  telemetry_ = std::make_shared<TelemType>(true);
}

template <typename TelemType>
ANLStatus InterpretTelemetry<TelemType>::mod_define() {
  define_parameter("save_telemetry", &mod_class::saveTelemetry_);
  define_parameter("num_telem_per_file", &mod_class::numTelemPerFile_);
  define_parameter("run_ID_filename", &mod_class::runIDFilename_);
  define_parameter("binary_filename_base", &mod_class::binaryFilenameBase_);
  define_parameter("receiver_module_name", &mod_class::receiverModuleName_);
  define_parameter("chatter", &mod_class::chatter_);
  return AS_OK;
}

template <typename TelemType>
ANLStatus InterpretTelemetry<TelemType>::mod_initialize() {
  timeStampStr_ = getTimeStr();
  if (exist_module(receiverModuleName_)) {
    get_module_NC(receiverModuleName_, &receiver_);
  }
#ifdef USE_HSQUICKLOOK
  const std::string pusher_module_name = "PushToMongoDB";
  if (exist_module(pusher_module_name)) {
    get_module_NC(pusher_module_name, &pusher_);
  }
#endif // USE_HSQUICKLOOK
  return AS_OK;
}

template <typename TelemType>
ANLStatus InterpretTelemetry<TelemType>::mod_analyze() {
  if (receiver_ == nullptr) {
    std::cerr << module_name() << "::mod_analyze: Receiver is nullptr" << std::endl;
    return AS_ERROR;
  }
  if (!(receiver_->Valid())) {
    currentTelemetryType_ = 0;
    return AS_OK;
  }

  currentTelemetryType_ = 0;
  const auto &telemetry = receiver_->Telemetry();
  if (!telemetry) {
    std::cerr << module_name() << "::mod_analyze: Telemery is nullptr" << std::endl;
    return AS_ERROR;
  }
  const bool status = interpret(telemetry);
  const bool failed = !status;
  if (failed) {
    std::cerr << module_name() << "::mod_analyze Failed to interpret telemetry..." << std::endl;
    writeTelemetryToFile(failed);
    return AS_OK;
  }
  if (saveTelemetry_) {
    writeTelemetryToFile(failed);
  }
  return AS_OK;
}

template <typename TelemType>
ANLStatus InterpretTelemetry<TelemType>::mod_finalize() {
  return AS_OK;
}

template <typename TelemType>
void InterpretTelemetry<TelemType>::writeTelemetryToFile(bool failed) {
  //int type = currentTelemetryType_;
  //std::string type_str = "";
  //if (type == 1) type_str = "HK";
  //if (type == 2) type_str = "WF";
  //if (type == 3) type_str = "Status";
  //if (failed) {
  //  type = 0;
  //  type_str = "failed";
  //}

  //const bool app = true;
  //if (fileIDmp_.find(type) == fileIDmp_.end()) {
  //  fileIDmp_[type] = std::pair<int, int>(0, 0);
  //}
  //else if (fileIDmp_[type].second == numTelemPerFile_) {
  //  fileIDmp_[type].first++;
  //  fileIDmp_[type].second = 0;
  //}

  //std::ostringstream id_sout;
  //id_sout << std::setfill('0') << std::right << std::setw(6) << fileIDmp_[type].first;
  //const std::string id_str = id_sout.str();
  //std::ostringstream run_id_sout;
  //run_id_sout << std::setfill('0') << std::right << std::setw(6) << currentRunID_;
  //const std::string run_id_str = run_id_sout.str();
  //const std::string filename = binaryFilenameBase_ + "_" + run_id_str + "_" + timeStampStr_ + "_" + type_str + "_" + id_str + ".dat";

  //if (!failed) {
  //  telemdef_->writeFile(filename, app);
  //}
  //else {
  //  writeVectorToBinaryFile<uint8_t>(filename, app, receiver_->Telemetry()->getContents()->Command());
  //}
  //timeval tv;
  //gettimeofday(&tv, NULL);
  //std::vector<int32_t> time_vec = {static_cast<int32_t>(tv.tv_sec), static_cast<int32_t>(tv.tv_usec)};
  //writeVectorToBinaryFile<int32_t>(filename, app, time_vec);
  //fileIDmp_[type].second++;
}

template <typename TelemType>
void InterpretTelemetry<TelemType>::updateRunIDFile() {
  const std::string time_stamp_str = getTimeStr();
  std::ofstream ofs(runIDFilename_, std::ios::app | std::ios::out);
  ofs << currentRunID_ << " " << time_stamp_str << "\n";
  ofs.flush();
  ofs.close();
}

template <typename TelemType>
bool InterpretTelemetry<TelemType>::interpret(const std::shared_ptr<BaseTelemetryDefinition> &telemDef) {
  if (!telemetry_) return false;
  const bool result = telemetry_->interpret(telemDef);
  if (chatter_ > 0) {
    telemetry_->print(std::cout);
  }
  return result;
}

template InterpretTelemetry<BaseTelemetryDefinition>::InterpretTelemetry();
template InterpretTelemetry<HubHKTelemetry>::InterpretTelemetry();
template ANLStatus InterpretTelemetry<BaseTelemetryDefinition>::mod_define();
template ANLStatus InterpretTelemetry<HubHKTelemetry>::mod_define();
template ANLStatus InterpretTelemetry<BaseTelemetryDefinition>::mod_initialize();
template ANLStatus InterpretTelemetry<HubHKTelemetry>::mod_initialize();
template ANLStatus InterpretTelemetry<BaseTelemetryDefinition>::mod_analyze();
template ANLStatus InterpretTelemetry<HubHKTelemetry>::mod_analyze();
template ANLStatus InterpretTelemetry<BaseTelemetryDefinition>::mod_finalize();
template ANLStatus InterpretTelemetry<HubHKTelemetry>::mod_finalize();

template void InterpretTelemetry<BaseTelemetryDefinition>::writeTelemetryToFile(bool);
template void InterpretTelemetry<HubHKTelemetry>::writeTelemetryToFile(bool);
template bool InterpretTelemetry<BaseTelemetryDefinition>::interpret(const std::shared_ptr<BaseTelemetryDefinition> &);
template bool InterpretTelemetry<HubHKTelemetry>::interpret(const std::shared_ptr<BaseTelemetryDefinition> &);
template void InterpretTelemetry<BaseTelemetryDefinition>::updateRunIDFile();
template void InterpretTelemetry<HubHKTelemetry>::updateRunIDFile();
} // namespace gramsballoon::pgrams
