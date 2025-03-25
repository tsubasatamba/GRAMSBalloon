#include "PassTelemetry.hh"
#include "SendTelemetry.hh"
using namespace anlnext;
namespace gramsballoon::pgrams {
ANLStatus PassTelemetry::mod_define() {
  define_parameter("MosquittoManager_name", &mod_class::mosquittoManagerName_);
  set_parameter_description("Name of MosquittoManager");
  define_parameter("topic", &mod_class::topic_);
  set_parameter_description("Topic to publish");
  define_parameter("DividePacket_name", &mod_class::dividePacketName_);
  set_parameter_description("Name of ReceiveStatusFromDAQComputer");
  define_parameter("chatter", &mod_class::chatter_);
  return AS_OK;
}
ANLStatus PassTelemetry::mod_initialize() {
  if (exist_module(mosquittoManagerName_)) {
    get_module_NC(mosquittoManagerName_, &mosquittoManager_);
  }
  else {
    std::cerr << "MosquittoManager module is not found." << std::endl;
    return AS_ERROR;
  }
  auto mosq = mosquittoManager_->getMosquittoIO();
  if (!mosq) {
    std::cerr << "MosquittoIO in the MosquittoManager is nullptr." << std::endl;
    return AS_ERROR;
  }
  if (exist_module(dividePacketName_)) {
    get_module_NC(dividePacketName_, &dividePacket_);
  }
  else {
    std::cerr << "ReceiveStatusFromDAQComputer module is not found." << std::endl;
    return AS_ERROR;
  }
  comdef_ = std::make_shared<CommandDefinition>();
  return AS_OK;
}
ANLStatus PassTelemetry::mod_analyze() {
  if (!mosquittoManager_) {
    std::cerr << "PassTelemetry::mod_analyze: MosquittoManager is nullptr." << std::endl;
    return AS_OK;
  }
  if (!dividePacket_) {
    std::cerr << "PassTelemetry::mod_analyze: ReceiveStatusFromDAQComputer is nullptr." << std::endl;
    return AS_OK;
  }
  auto mosq = mosquittoManager_->getMosquittoIO();
  if (!mosq) {
    std::cerr << "PassTelemetry::mod_analyze: MosquittoIO in the MosquittoManager is nullptr." << std::endl;
    return AS_OK;
  }
  if (!comdef_) {
    std::cerr << "PassTelemetry::mod_analyze: CommandDefinition is nullptr." << std::endl;
    return AS_OK;
  }
  if (dividePacket_->IsEmpty()) {
    return AS_OK;
  }
  auto &packet = dividePacket_->GetLastPacket();
  const auto result = comdef_->setCommand(packet);
  if (!result) {
    std::cerr << "PassTelemetry::mod_analyze: Command is invalid." << std::endl;
    dividePacket_->PopPacket();
    return AS_ERROR;
  }
  comdef_->interpret();
  std::cout << "Command code: " << comdef_->Code() << std::endl;
  std::cout << "Number of arguments: " << comdef_->Argc() << std::endl;
  const auto &args = comdef_->Arguments();
  for (const auto &arg: args) {
    std::cout << "Argument: " << arg << std::endl;
  }
  std::cout << std::endl;
  dividePacket_->PopPacket();
  return AS_OK;
}
} // namespace gramsballoon::pgrams