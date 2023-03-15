/**
 * Regulate high voltage via Analog Discovery.
 *
 * @author Tsubasa Tamba, Shota Arai
 * @date 2023-03-15
 */


#ifndef RegulateHighVoltage_H
#define RegulateHighVoltage_H 1

#include <anlnext/BasicModule.hh>
#include "AnalogDiscoveryManager.hh"

class RegulateHighVoltage : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(RegulateHighVoltage, 1.0);

public:
  RegulateHighVoltage();
  virtual ~RegulateHighVoltage();
  
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_finalize() override;

  void setNextVoltage(double v) { nextVoltage_ = v; }

private:
  int deviceID_ = 0;
  int channel_ = 0;
  std::string ADManagerName_ = "AnalogDiscoveryManager";
  AnalogDiscoveryManager* ADManager_ = nullptr;
  int sleep_ = 500;
  double currentVoltage_ = 0.0;
  double nextVoltage_ = 0.0;
};

#endif /* RegulateHighVoltage_H */
