/**
 * GetRaspiStatus sample module of the ANL Next framework
 *
 * @author 
 * @date
 *
 */

#ifndef GetRaspiStatus_H
#define GetRaspiStatus_H 1

#include <anlnext/BasicModule.hh>
#include <cstdint>
#include <fstream>

namespace GRAMSBalloon {

class GetRaspiStatus : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(GetRaspiStatus, 1.0);
public:
  GetRaspiStatus();
  virtual ~GetRaspiStatus();
  
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_finalize() override;

  int TemperatureADC() { return temperatureADC_; }

private:
  int temperatureADC_;
  std::ifstream ifsTemp_;
};

} /* namespace GRAMSBalloon */

#endif /* GetRaspiStatus_H */
