#include "InterpretHKTelemetry.hh"
using namespace anlnext;

namespace gramsballoon {

InterpretHKTelemetry::InterpretHKTelemetry()
{
  telemdef_ = std::make_shared<TelemetryDefinition>();
}

InterpretHKTelemetry::~InterpretHKTelemetry() = default;

ANLStatus InterpretHKTelemetry::mod_define()
{
  //define_parameter("baudrate", &mod_class::baudrate_);
  
  return AS_OK;
}

ANLStatus InterpretHKTelemetry::mod_initialize()
{
  const std::string receiver_module_name = "ReceiveTelemetry";
  if (exist_module(receiver_module_name)) {
    get_module_NC(receiver_module_name, &receiver_);
  }
  
  return AS_OK;
}

ANLStatus InterpretHKTelemetry::mod_analyze()
{
  const std::vector<uint8_t>& telemetry = receiver_->Telemetry();

  // length is valid or not

  bool status = telemdef_->setTelemetry(telemetry);
  if (!status) {
    std::cerr << "InterpretHKTelemetry::mod_analyze Failed to set telemetry..." << std::endl;
    return AS_OK;
  }
  telemdef_->interpret();
  std::cout << "event id: " << telemdef_->EventID() << std::endl;
  
  return AS_OK;
}

ANLStatus InterpretHKTelemetry::mod_finalize()
{
  return AS_OK;
}

} // namespace gramsballoon
