#include "ControlHighVoltage.hh"
#include <chrono>
#include <thread>

using namespace anlnext;

namespace gramsballoon {

ControlHighVoltage::ControlHighVoltage()
{
}

ControlHighVoltage::~ControlHighVoltage() = default;

ANLStatus ControlHighVoltage::mod_define()
{
  define_parameter("device_id", &mod_class::deviceID_);
  define_parameter("channel", &mod_class::channel_);
  define_parameter("sleep", &mod_class::sleep_);
  define_parameter("voltages", &mod_class::voltages_);
  define_parameter("chatter", &mod_class::chatter_);

  return AS_OK;
}

ANLStatus ControlHighVoltage::mod_initialize()
{
  const std::string send_telem_md = "SendTelemetry";
  if (exist_module(send_telem_md)) {
    get_module_NC(send_telem_md, &sendTelemetry_);
  }

  if (exist_module(ADManagerName_)) {
    get_module_NC(ADManagerName_, &ADManager_);
  }
  else {
    std::cerr << "Error in ControlHighVoltage::mod_initialize." << std::endl;
    std::cerr << "Analog Discovery manager does not exist. Module name = " << ADManagerName_ << std::endl;
    if (sendTelemetry_) {
      sendTelemetry_->getErrorManager()->setError(ErrorType::MODULE_ACCESS_ERROR);
    }
  }
  AnalogDiscoveryIO* io = ADManager_->ADIO();
  io -> setupAnalogOut(deviceID_, channel_);
  
  return AS_OK;
}

ANLStatus ControlHighVoltage::mod_analyze()
{
  AnalogDiscoveryIO* adio = ADManager_->ADIO();
  const int num_devices = adio->NumDevices();
  if (num_devices<=deviceID_) {
    setInvalidChannelError();
    return AS_OK;
  }
  if (channel_<0 || channel_>=2) {
    setInvalidChannelError();
    return AS_OK;
  }
  
  if (voltageIndex_<static_cast<int>(voltages_.size())) {
    exec_ = true;
    nextVoltage_ = voltages_[voltageIndex_];
    voltageIndex_++;
  }
  
  if ((currentVoltage_!=nextVoltage_) && exec_) {
    AnalogDiscoveryIO* io = ADManager_->ADIO();
    io -> setVoltage(deviceID_, channel_, static_cast<double>(nextVoltage_), sleep_);
    currentVoltage_ = nextVoltage_;
    exec_ = false;
  }
  
  return AS_OK;
}

ANLStatus ControlHighVoltage::mod_finalize()
{
  return AS_OK;
}

void ControlHighVoltage::setInvalidChannelError()
{
  if (sendTelemetry_==nullptr) return;

  const std::string name = module_id();
  if (name=="ControlHighVoltage_TPC") {
    sendTelemetry_->getErrorManager()->setError(ErrorType::TPC_HV_INVALID_CHANNEL);
  }
  else if (name=="ControlHighVoltage_PMT") {
    sendTelemetry_->getErrorManager()->setError(ErrorType::PMT_HV_INVALID_CHANNEL);
  }
  else {
    sendTelemetry_->getErrorManager()->setError(ErrorType::OTHER_ERRORS);
  }
}

} /* namespace gramsballoon */
