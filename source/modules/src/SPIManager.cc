#include "SPIManager.hh"
#include "pigpiod_if2.h"
#include "bme68x.h"


using namespace anlnext;

SPIManager::SPIManager()
{
}

SPIManager::~SPIManager() = default;

ANLStatus SPIManager::mod_define()
{
  define_parameter("channel", &mod_class::channel_);
  define_parameter("baudrate", &mod_class::baudrate_);
  define_parameter("spi_flags", &mod_class::spiFlags_);

  return AS_OK;
}

ANLStatus SPIManager::mod_pre_initialize()
{
  return AS_OK;
}

ANLStatus SPIManager::mod_initialize()
{
  int pi = pigpio_start(NULL, portNumber_.c_str());
  if (pi<0) {
    std::cerr << "pigpio daemon connection failed: pi =  " << pi << std::endl;
    return AS_QUIT_ERROR;
  }
  if (chipSelect_<0 || chipSelect_>=27) {
    std::cerr << "Chip select must be non-negative and smaller than 27: CS=" << chipSelect_ << std::endl;
    return AS_QUIT_ERROR;
  }

  unsigned int spi_handler = spi_open(pi, channel_, baudrate_, spiFlags_);
  if (spi_handler<0) {
    std::cerr << "spi open failed: spi_handler = " << spi_handler << std::endl;
    return AS_QUIT_ERROR;
  }

  interface_ -> setGPIOHandler(pi);
  interface_ -> setChipSelect(chipSelect_);
  interface_ -> setSPIHandler(spi_handler);
  
  set_mode(interface_->GPIOHandler(), interface_->ChipSelect(), PI_OUTPUT);
  return AS_OK;
}

ANLStatus SPIManager::mod_begin_run()
{
  return AS_OK;
}

ANLStatus SPIManager::mod_analyze()
{
  return AS_OK;
}

ANLStatus SPIManager::mod_end_run()
{
  return AS_OK;
}

ANLStatus SPIManager::mod_finalize()
{
  spi_close(interface_->GPIOHandler(), interface_->SPIHandler());
  pigpio_stop(interface_->GPIOHandler());
  
  return AS_OK;
}
