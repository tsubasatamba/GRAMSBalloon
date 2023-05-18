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
  currentTelemetryType_ = 0;
  const std::vector<uint8_t>& telemetry = receiver_->Telemetry();

  bool status = telemdef_->setTelemetry(telemetry);
  if (!status) {
    std::cerr << "InterpretTelemetry::mod_analyze Failed to set telemetry..." << std::endl;
    return AS_OK;
  }
  telemdef_->interpret();
  currentTelemetryType_ = telemdef_->TelemetryType();

  writeTelemetryToFile();
  
  return AS_OK;
}

ANLStatus InterpretTelemetry::mod_finalize()
{
  return AS_OK;
}

void InterpretTelemetry::writeTelemetryToFile()
{
  int type = currentTelemetryType_;
  std::string type_str = "";
  if (type==1) type_str = "HK";
  if (type==2) type_str = "WF";
  if (type==3) type_str = "Status";

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
  
  telemdef_->writeFile(filename, app);
  fileIDmp_[type].second++;
}

} // namespace gramsballoon
