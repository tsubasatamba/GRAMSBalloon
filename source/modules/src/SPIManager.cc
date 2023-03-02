#include "SPIManager.hh"
#include "pigpiod_if2.h"
#include "bme68x.h"


using namespace anlnext;

SPIManager::SPIManager()
{
  interface_ = std::make_unique<SPIInterface>();
}

SPIManager::~SPIManager() = default;

ANLStatus SPIManager::mod_define()
{
  define_parameter("channel", &mod_class::channel_);
  define_parameter("baudrate", &mod_class::baudrate_);
  define_parameter("spi_flags", &mod_class::spiFlags_);

  return AS_OK;
}

ANLStatus SPIManager::mod_initialize()
{
  int pi = pigpio_start(NULL, portNumber_.c_str());
  if (pi<0) {
    std::cerr << "pigpio daemon connection failed: pi =  " << pi << std::endl;
    return AS_QUIT_ERROR;
  }

  unsigned int spi_handler = spi_open(pi, channel_, baudrate_, spiFlags_);
  if (static_cast<int>(spi_handler)<0) {
    std::cerr << "spi open failed: spi_handler = " << spi_handler << std::endl;
    return AS_QUIT_ERROR;
  }

  interface_ -> setGPIOHandler(pi);
  interface_ -> setChipSelect(8);
  interface_ -> setSPIHandler(spi_handler);

  const int n = chipSelectArray_.size();
  std::cout << "n=" << n << std::endl;
  for (int i=0; i<n; i++) {
    set_mode(pi, chipSelectArray_[i], PI_OUTPUT);
    set_pull_up_down(pi, chipSelectArray_[i], PI_PUD_UP);
    gpio_write(pi, chipSelectArray_[i], CS_DISABLE);
  }
   
  return AS_OK;
}

ANLStatus SPIManager::mod_analyze()
{
  return AS_OK;
}

ANLStatus SPIManager::mod_finalize()
{
  spi_close(interface_->GPIOHandler(), interface_->SPIHandler());
  pigpio_stop(interface_->GPIOHandler());
  
  return AS_OK;
}

void SPIManager::addChipSelect(int v)
{
  chipSelectArray_.push_back(v);
}
