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

  int getCapacity();

  int CPUTemperatureADC() { return CPUTemperatureADC_; }
  double CPUTemperature() { return CPUTemperature_; }
  uint64_t CapacityFree() { return capacityFree_; }
  uint64_t CapacityAll() { return capacityAll_; }

private:
  int CPUTemperatureADC_;
  double CPUTemperature_;
  std::string tempFile_;
  std::string path_;
  uint64_t capacityFree_ = 0;
  uint64_t capacityAll_ = 0;
};

} /* namespace gramsballoon */

#endif /* GetRaspiStatus_H */
