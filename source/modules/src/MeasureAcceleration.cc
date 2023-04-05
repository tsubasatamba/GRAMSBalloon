#include "MeasureAcceleration.hh"

using namespace anlnext;

namespace gramsballoon {

MeasureAcceleration::MeasureAcceleration()
{
  icmIO_ = std::make_shared<ICM20948IO>();
  devicePath_ = "/dev/i2c-1";
}

MeasureAcceleration::~MeasureAcceleration() = default;

ANLStatus MeasureAcceleration::mod_define()
{
  define_parameter("device_path", &mod_class::devicePath_);
  define_parameter("calibrateGyro", &mod_class::calibrateGyro_);
  return AS_OK;
}

ANLStatus MeasureAcceleration::mod_initialize()
{
  const bool status = icmIO_->initialize();
  if (!status) {
    return AS_ERROR;
  }
  if (calibrateGyro_) {
    icmIO_->calibrateGyro();
  }
  return AS_OK;
}

ANLStatus MeasureAcceleration::mod_analyze()
{
  icmIO_->measure();

  #if 1
  const IMUData* data = getData();
  std::cout << "Accel: ";
  for (int j = 0; j < 3; j++) {
    std::cout << data->mAcc[j] << " ";
  }
  std::cout << std::endl;
  std::cout << "Gyro(dps): ";
  for (int j = 0; j < 3; j++) {
    std::cout << data->mGyro[j] << " ";
  }
  std::cout << std::endl;
  std::cout << "Magnet[uT]";
  for (int j = 0; j < 3; j++) {
    std::cout << data->mMag[j] << " ";
  }
  std::cout << std::endl;
  #endif

  return AS_OK;
}

ANLStatus MeasureAcceleration::mod_finalize()
{
  return AS_OK;
}

} /* namespace gramsballoon */