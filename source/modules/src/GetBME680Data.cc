#include "GetBME680Data.hh"
#include "pigpiod_if2.h"
#include "bme68x.h"


using namespace anlnext;

GetBME680Data::GetBME680Data()
{
  bme680io_ = std::make_unique<BME680IO>();
  interface_ = std::make_unique<SPIInterface>();
}

GetBME680Data::~GetBME680Data() = default;

ANLStatus GetBME680Data::mod_define()
{
  define_parameter("chip_select", &mod_class::chipSelect_);
  define_parameter("SPI_manager_name", &mod_class::SPIManagerName_);

  return AS_OK;
}

ANLStatus GetBME680Data::mod_initialize()
{
  get_module_NC(SPIManagerName_, &SPIManager_);
  const unsigned int spihandler = SPIManager_ -> Interface() -> SPIHandler();
  const int pi = SPIManager_ -> Interface() -> GPIOHandler();
  
  interface_ -> setSPIHandler(spihandler);
  interface_ -> setChipSelect(chipSelect_);
  interface_ -> setGPIOHandler(pi);

  bme680io_ -> setup(interface_.get());
  
  return AS_OK;
}

ANLStatus GetBME680Data::mod_analyze()
{
  bme680io_ -> getData();
  bme680io_ -> printData();
  
  return AS_OK;
}

ANLStatus GetBME680Data::mod_finalize()
{  
  return AS_OK;
}
