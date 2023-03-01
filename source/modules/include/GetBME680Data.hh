/**
 * Get environmental data with BME680 sensor.
 *
 * @author Tsubasa Tamba, Shota Arai
 * @date 2023-03-01
 */


#ifndef GetBME680Data_H
#define GetBME680Data_H 1

#include <anlnext/BasicModule.hh>
#include "SPIInterface.hh"
#include "BME680IO.hh"
#include "SPIManager.hh"

class GetBME680Data : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(GetBME680Data, 1.0);

public:
  GetBME680Data();
  virtual ~GetBME680Data();
  
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_finalize() override;

private:
  int chipSelect_ = 8;
  std::string SPIManagerName_ = "SPIManager";
  SPIManager* SPIManager_ = nullptr;
  std::unique_ptr<BME680IO> bme680io_ = nullptr;
  std::unique_ptr<SPIInterface> interface_ = nullptr;
};

#endif /* GetBME680Data_H */
