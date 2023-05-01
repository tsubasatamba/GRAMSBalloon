/**
 * Get environmental data with BME680 sensor.
 *
 * @author Tsubasa Tamba, Shota Arai
 * @date 2023-03-20
 */


#ifndef GetSlowADCData_H
#define GetSlowADCData_H 1

#include <anlnext/BasicModule.hh>
#include "SlowADCIO.hh"
#include "SPIManager.hh"

namespace gramsballoon {

class GetSlowADCData : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(GetSlowADCData, 1.0);
  ENABLE_PARALLEL_RUN();

public:
  GetSlowADCData();
  virtual ~GetSlowADCData();
protected:
  GetSlowADCData(const GetSlowADCData& r) = default;

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_pre_initialize() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_finalize() override;

  uint16_t getADC(int channel) { return adcList_[channel]; }
  double getVoltage(int channel) { return voltageList_[channel]; }

private:
  int chipSelect_ = 8;
  std::string SPIManagerName_ = "SPIManager";
  SPIManager* SPIManager_ = nullptr;
  std::shared_ptr<SlowADCIO> slowADCio_ = nullptr;
  std::shared_ptr<SPIInterface> interface_ = nullptr;
  double va_ = 5.0;
  std::vector<int> channels_;
  std::map<int, uint16_t> adcList_;
  std::map<int, double> voltageList_;
};

} /* namespace gramsballoon */

#endif /* GetSlowADCData_H */
