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

namespace gramsballoon {

class GetRaspiStatus : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(GetRaspiStatus, 1.0);
  ENABLE_PARALLEL_RUN();
public:
  GetRaspiStatus();
  virtual ~GetRaspiStatus();
protected:
  GetRaspiStatus(const GetRaspiStatus& r) = default;

public:  
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_finalize() override;

  int TemperatureADC() { return temperatureADC_; }

private:
  int temperatureADC_;
  std::shared_ptr<std::ifstream> ifsTemp_;
};

} /* namespace gramsballoon */

#endif /* GetRaspiStatus_H */
