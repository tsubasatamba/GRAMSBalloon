/**
 * Control high voltage via Analog Discovery.
 *
 * @author Tsubasa Tamba, Shota Arai
 * @date 2023-03-15
 */


#ifndef ControlHighVoltage_H
#define ControlHighVoltage_H 1

#include <anlnext/BasicModule.hh>
#include "AnalogDiscoveryManager.hh"

namespace GRAMSBalloon {

class ControlHighVoltage : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(ControlHighVoltage, 1.0);

public:
  ControlHighVoltage();
  virtual ~ControlHighVoltage();
  
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

} /* namespace GRAMSBalloon */

#endif /* ControlHighVoltage_H */
