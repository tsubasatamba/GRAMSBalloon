#include "RegulateHighVoltage.hh"


using namespace anlnext;

RegulateHighVoltage::RegulateHighVoltage()
{
}

RegulateHighVoltage::~RegulateHighVoltage() = default;

ANLStatus RegulateHighVoltage::mod_define()
{
  define_parameter("device_id", &mod_class::deviceID_);
  define_parameter("channel", &mod_class::channel_);
  define_parameter("sleep", &mod_class::sleep_);

  return AS_OK;
}

ANLStatus RegulateHighVoltage::mod_initialize()
{
  get_module_NC(ADManagerName_, &ADManager_);
  AnalogDiscoveryIO* io = ADManager_->ADIO();
  io -> setup(deviceID_, channel_);
  
  return AS_OK;
}

ANLStatus RegulateHighVoltage::mod_analyze()
{
  # if 0
  const int index = get_loop_index();
  if (index==1) {
    nextVoltage_ = 2.0;
  }
  if (index==6) {
    nextVoltage_ = 4.0;
  }
  # endif
  
  if (currentVoltage_!=nextVoltage_) {
    AnalogDiscoveryIO* io = ADManager_->ADIO();
    io -> setVoltage(deviceID_, channel_, static_cast<double>(nextVoltage_), sleep_);
    currentVoltage_ = nextVoltage_;
  }
  
  return AS_OK;
}

ANLStatus RegulateHighVoltage::mod_finalize()
{
  return AS_OK;
}
