/**
 * Get environmental data with BME680 sensor.
 *
 * @author Tsubasa Tamba, Shota Arai
 * @date 2023-03-20
 */


#ifndef MeasureTemperatureWithRTDSensor_H
#define MeasureTemperatureWithRTDSensor_H 1

#include <anlnext/BasicModule.hh>
#include "SPIInterface.hh"
#include "MAX31865IO.hh"
#include "SPIManager.hh"
#include <chrono>

namespace GRAMSBalloon {

class MeasureTemperatureWithRTDSensor : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(MeasureTemperatureWithRTDSensor, 1.0);
  ENABLE_PARALLEL_RUN();

public:
  MeasureTemperatureWithRTDSensor();
  virtual ~MeasureTemperatureWithRTDSensor();
protected:
  MeasureTemperatureWithRTDSensor(const MeasureTemperatureWithRTDSensor& r) = default;

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_pre_initialize() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_finalize() override;

  MAX31865IO* GetMAX31865IO() { return max31865io_.get(); }
  int16_t TemperatureADC() { return max31865io_->TemperatureADC(); }

private:
  int chipSelect_ = 8;
  std::string SPIManagerName_ = "SPIManager";
  SPIManager* SPIManager_ = nullptr;
  std::shared_ptr<MAX31865IO> max31865io_ = nullptr;
  std::shared_ptr<SPIInterface> interface_ = nullptr;
};

} /* namespace GRAMSBalloon */

#endif /* MeasureTemperatureWithRTDSensor_H */
