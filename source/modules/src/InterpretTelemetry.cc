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
  
  return AS_OK;
}

ANLStatus InterpretTelemetry::mod_finalize()
{
  return AS_OK;
}


} // namespace gramsballoon
