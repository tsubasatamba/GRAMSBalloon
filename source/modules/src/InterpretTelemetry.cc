#include "InterpretTelemetry.hh"
#include "DateManager.hh"

using namespace anlnext;

namespace gramsballoon {

InterpretTelemetry::InterpretTelemetry()
{
  telemdef_ = std::make_shared<TelemetryDefinition>();
  binaryFilenameBase_ = "Telemetry";
}

InterpretTelemetry::~InterpretTelemetry() = default;

ANLStatus InterpretTelemetry::mod_define()
{
  define_parameter("save_telemetry", &mod_class::saveTelemetry_);
  define_parameter("num_telem_per_file", &mod_class::numTelemPerFile_);
  define_parameter("chatter", &mod_class::chatter_);
  
  return AS_OK;
}

ANLStatus InterpretTelemetry::mod_initialize()
{
  timeStampStr_ = getTimeStr();

  const std::string receiver_module_name = "ReceiveTelemetry";
  if (exist_module(receiver_module_name)) {
    get_module_NC(receiver_module_name, &receiver_);
  }
  
  return AS_OK;
}

ANLStatus InterpretTelemetry::mod_analyze()
{
  if (!receiver_->Valid()) {
    return AS_OK;
  }

  currentTelemetryType_ = 0;
  const std::vector<uint8_t>& telemetry = receiver_->Telemetry();

  bool status = telemdef_->setTelemetry(telemetry);
  const bool failed = !status;
  if (!status) {
    std::cerr << "InterpretTelemetry::mod_analyze Failed to set telemetry..." << std::endl;
    writeTelemetryToFile(failed);
    return AS_OK;
  }
  telemdef_->interpret();
  currentTelemetryType_ = telemdef_->TelemetryType();

  writeTelemetryToFile(failed);
  
  return AS_OK;
}

ANLStatus InterpretTelemetry::mod_finalize()
{
  return AS_OK;
}

void InterpretTelemetry::writeTelemetryToFile(bool failed)
{
  int type = currentTelemetryType_;
  std::string type_str = "";
  if (type==1) type_str = "HK";
  if (type==2) type_str = "WF";
  if (type==3) type_str = "Status";
  if (failed) {
    type = 0;
    type_str = "failed";
  }

  const bool app = true;
  if (fileIDmp_.find(type)==fileIDmp_.end()) {
    fileIDmp_[type] = std::pair<int, int>(0, 0);
  }
  else if (fileIDmp_[type].second==numTelemPerFile_) {
    fileIDmp_[type].first++;
    fileIDmp_[type].second = 0;
  }

  std::ostringstream sout;
  sout << std::setfill('0') << std::right << std::setw(6) << fileIDmp_[type].first;
  const std::string id_str = sout.str();
  const std::string filename = binaryFilenameBase_ + "_" + timeStampStr_ + "_" + type_str + "_" + id_str + ".dat";
  
  if (!failed) {
    telemdef_->writeFile(filename, app);
  }
  else {
    writeVectorToBinaryFile(filename, app, receiver_->Telemetry());
  }
  fileIDmp_[type].second++;
}

} // namespace gramsballoon
