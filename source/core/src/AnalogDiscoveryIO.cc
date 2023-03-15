#include "AnalogDiscoveryIO.hh"
#include <thread>
#include <chrono>

AnalogDiscoveryIO::AnalogDiscoveryIO()
{
  bme68xn_ = std::make_unique<bme68x_dev>();
  sensorData_ = std::make_unique<bme68x_data>();
  configure_ = std::make_unique<bme68x_conf>();
}

int AnalogDiscoveryIO::initialize()
{
  if (!FDwfEnum(enumfilterAll, &numDevices_)) {
    FDwfGetLastErrorMsg(szError_);
    printf("FDwfEnum: %s\n", szError_);
    std::cout << "FDwfEnum failed: " << szError_ << std::endl;
    return -1;
  }

  handlerList_.resize(numDevices_);

  for (int i=0; i<numDevices_; i++) {
    if (!FDwfDeviceOpen(i, &handlerList_[i])) {
      FDwfGetLastErrorMsg(szError_);
      std::cerr << "Device open failed: device id = " << i << ",\n" << szError_ << std::endl;
      return -1;
    }
  }

  return 0;
}

void AnalogDiscovery::setup(int device_id, int channel, double init_value*/=0.0*/)
{
  FDwfAnalogOutNodeEnableSet(handlerList_[device_id], channel, AnalogOutNodeCarrier, true);
  FDwfAnalogOutNodeFunctionSet(handlerList_[device_id], channel, AnalogOutNodeCarrier, funcDC);
  FDwfAnalogOutNodeOffsetSet(handlerList_[device_id], channel, AnalogOutNodeCarrier, init_value);
  FDwfAnalogOutConfigure(handlerList_[device_id], channel, ANALOG_OUT_START);
}

void AnalogDiscovery::setVoltage(int device_id, int channel, double voltage, double sleep)
{
  FDwfAnalogOutNodeOffsetSet(handlerList_[device_id], channel, AnalogOutNodeCarrier, voltage);
  FDwfAnalogOutConfigure(handlerList_[device_id], channel, ANALOG_OUT_APPLY);
  std::this_thread::sleep_for(std::chrono::seconds(sleep));
}

void AnalogDiscoveryIO::finalize()
{
  FDwfDeviceCloseAll();
}






