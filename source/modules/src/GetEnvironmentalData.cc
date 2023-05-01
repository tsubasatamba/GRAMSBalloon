#include <thread>
#include <chrono>
#include "GetEnvironmentalData.hh"
#include "pigpiod_if2.h"
#include "bme68x.h"

using namespace anlnext;

namespace gramsballoon {

GetEnvironmentalData::GetEnvironmentalData()
{
  bme680io_ = std::make_shared<BME680IO>();
  interface_ = std::make_shared<SPIInterface>();
}

GetEnvironmentalData::~GetEnvironmentalData() = default;

ANLStatus GetEnvironmentalData::mod_define()
{
  define_parameter("chip_select", &mod_class::chipSelect_);
  define_parameter("SPI_manager_name", &mod_class::SPIManagerName_);

  return AS_OK;
}

ANLStatus GetEnvironmentalData::mod_pre_initialize()
{
  if (chipSelect_<0 || chipSelect_>=27) {
    std::cerr << "Chip select must be non-negative and smaller than 27: CS=" << chipSelect_ << std::endl;
    return AS_QUIT_ERROR;
  }
  if (exist_module(SPIManagerName_)) {
    get_module_NC(SPIManagerName_, &SPIManager_);
  }
  else {
    std::cerr << "SPI manager does not exist. Module name = " << SPIManagerName_ << std::endl;
    return AS_QUIT_ERROR;
  }
  SPIManager_->addChipSelect(chipSelect_);
  
  return AS_OK;
}

ANLStatus GetEnvironmentalData::mod_initialize()
{
  const unsigned int spihandler = SPIManager_ -> Interface() -> SPIHandler();
  const int pi = SPIManager_ -> Interface() -> GPIOHandler();
  
  interface_ -> setSPIHandler(spihandler);
  interface_ -> setChipSelect(chipSelect_);
  interface_ -> setGPIOHandler(pi);

  bme680io_ -> setup(interface_.get());
  
  return AS_OK;
}

ANLStatus GetEnvironmentalData::mod_analyze()
{
  const int status = bme680io_ -> getData();
  if (status!=0) {
    std::cerr << "Failed to get environmental data." << std::endl;
    return AS_ERROR;
  }
  bme680io_ -> printData();
  pressure_ = bme680io_ -> SensorData() -> pressure;
  humidity_ = bme680io_ -> SensorData() -> humidity;
  temperature_ = bme680io_ -> SensorData() -> temperature;

  return AS_OK;
}

ANLStatus GetEnvironmentalData::mod_finalize()
{  
  return AS_OK;
}

} /* namespace gramsballoon */
