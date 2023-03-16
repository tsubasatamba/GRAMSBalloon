/**
 * Get environmental data with BME680 sensor.
 *
 * @author Tsubasa Tamba, Shota Arai
 * @date 2023-03-01
 */


#ifndef GetEnvironmentalData_H
#define GetEnvironmentalData_H 1

#include <anlnext/BasicModule.hh>
#include "SPIInterface.hh"
#include "BME680IO.hh"
#include "SPIManager.hh"
#include <chrono>

class GetEnvironmentalData : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(GetEnvironmentalData, 1.0);

public:
  GetEnvironmentalData();
  virtual ~GetEnvironmentalData();
  
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_pre_initialize() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_finalize() override;

  

private:
  int chipSelect_ = 8;
  std::string SPIManagerName_ = "SPIManager";
  SPIManager* SPIManager_ = nullptr;
  std::unique_ptr<BME680IO> bme680io_ = nullptr;
  std::unique_ptr<SPIInterface> interface_ = nullptr;
  double pressure_;
  double humidity_;
  double temperature_;
  std::chrono::system_clock::time_point lastUpdateTime_;
};

#endif /* GetEnvironmentalData_H */
