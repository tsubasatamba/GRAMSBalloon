#include <thread>
#include <chrono>
#include "MeasureTemperatureWithRTDSensor.hh"
#include "pigpiod_if2.h"
#include "bme68x.h"

using namespace anlnext;

namespace gramsballoon {

MeasureTemperatureWithRTDSensor::MeasureTemperatureWithRTDSensor()
{
  max31865io_ = std::make_shared<MAX31865IO>();
  interface_ = std::make_shared<SPIInterface>();
}

MeasureTemperatureWithRTDSensor::~MeasureTemperatureWithRTDSensor() = default;

ANLStatus MeasureTemperatureWithRTDSensor::mod_define()
{
  define_parameter("chip_select", &mod_class::chipSelect_);
  define_parameter("SPI_manager_name", &mod_class::SPIManagerName_);

  return AS_OK;
}

ANLStatus MeasureTemperatureWithRTDSensor::mod_pre_initialize()
{
  if (chipSelect_<0 || chipSelect_>=27) {
    std::cerr << "Chip select must be non-negative and smaller than 27: CS=" << chipSelect_ << std::endl;
    return AS_QUIT_ERROR;
  }
  get_module_NC(SPIManagerName_, &SPIManager_);
  SPIManager_->addChipSelect(chipSelect_);
  
  return AS_OK;
}

ANLStatus MeasureTemperatureWithRTDSensor::mod_initialize()
{
  const unsigned int spihandler = SPIManager_ -> Interface() -> SPIHandler();
  const int pi = SPIManager_ -> Interface() -> GPIOHandler();
  
  interface_ -> setSPIHandler(spihandler);
  interface_ -> setChipSelect(chipSelect_);
  interface_ -> setGPIOHandler(pi);

  max31865io_->setInterface(interface_.get());

  max31865io_->faultStatusClear();
  max31865io_->setConfigureSingle(MAX31865_CONF_BIAS_ON, MAX31865_CONF_BIAS_MSK);
  max31865io_->setConfigureSingle(MAX31865_CONF_CONVERSION_AUTO, MAX31865_CONF_CONVERSION_MSK);
  max31865io_->setConfigureSingle(MAX31865_CONF_MANUAL_FD_DISABLE, MAX31865_CONF_FD_CYCLE_MSK);
  max31865io_->setConfigureSingle(MAX31865_CONF_3WIRE, MAX31865_CONF_WIRE_MSK);
  max31865io_->setConfigureSingle(MAX31865_CONF_50HZ, MAX31865_CONF_FILTER_SEL_MSK);

  return AS_OK;
}

ANLStatus MeasureTemperatureWithRTDSensor::mod_analyze()
{
  int status = max31865io_->getData();
  if (status!=MAX31865_OK) {
    return AS_SKIP;
  }

  #if 0
  double temperature = max31865io_->Temperature();
  std::cout << "temperature ADC: " << TemperatureADC() << std::endl;
  std::cout << " temperature : " << temperature << std::endl;
  #endif
  
  return AS_OK;
}

ANLStatus MeasureTemperatureWithRTDSensor::mod_finalize()
{  
  return AS_OK;
}

} /* namespace gramsballoon */