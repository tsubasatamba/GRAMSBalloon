#include "GetSlowADCData.hh"
#include <chrono>
#include <thread>

using namespace anlnext;

namespace gramsballoon {

GetSlowADCData::GetSlowADCData()
{
  slowADCio_ = std::make_shared<SlowADCIO>();
  interface_ = std::make_shared<SPIInterface>();
  channels_ = {0};
}

GetSlowADCData::~GetSlowADCData() = default;

ANLStatus GetSlowADCData::mod_define()
{
  define_parameter("chip_select", &mod_class::chipSelect_);
  define_parameter("SPI_manager_name", &mod_class::SPIManagerName_);
  define_parameter("Va", &mod_class::va_);
  define_parameter("channels", &mod_class::channels_);
  define_parameter("num_trials", &mod_class::numTrials_);

  return AS_OK;
}

ANLStatus GetSlowADCData::mod_pre_initialize()
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

  if (numTrials_<2) {
    numTrials_ = 2;
  }

  std::sort(channels_.begin(), channels_.end());
  channels_.erase(std::unique(channels_.begin(), channels_.end()), channels_.end());

  return AS_OK;
}

ANLStatus GetSlowADCData::mod_initialize()
{
  const unsigned int spihandler = SPIManager_ -> Interface() -> SPIHandler();
  const int pi = SPIManager_ -> Interface() -> GPIOHandler();

  interface_ -> setSPIHandler(spihandler);
  interface_ -> setChipSelect(chipSelect_);
  interface_ -> setGPIOHandler(pi);

  slowADCio_->setInterface(interface_.get());
  slowADCio_->setVa(va_);

  return AS_OK;
}

ANLStatus GetSlowADCData::mod_analyze()
{
  {
    uint16_t adc = 0;
    double voltage = 0.0;
    slowADCio_->getData(0, adc, voltage, numTrials_);
  }
  for (int channel: channels_) {
    uint16_t adc = 0;
    double voltage = 0.0;
    const bool status = slowADCio_->getData(channel, adc, voltage, numTrials_);
    if (status) {
      adcList_[channel] = adc;
      voltageList_[channel] = voltage;
    }
    else {
      std::cerr << "Error in GetSlowADCData::mod_analyze. Failed to get data." << std::endl;
    }
  }

  #if 1
  std::cout << "channel adc" << std::endl;
  for (auto m: adcList_) {
    std::cout << m.first << " " << m.second << std::endl;
  }
  std::cout << "channel voltage" << std::endl;
  for (auto m: voltageList_) {
    std::cout << m.first << " " << m.second << std::endl;
  }
  #endif
 
  return AS_OK;
}

ANLStatus GetSlowADCData::mod_finalize()
{
  return AS_OK;
}

} /* namespace gramsballoon */
