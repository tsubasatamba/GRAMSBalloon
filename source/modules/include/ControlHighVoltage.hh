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

namespace gramsballoon {

class ControlHighVoltage : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(ControlHighVoltage, 1.0);
  ENABLE_PARALLEL_RUN(); 

public:
  ControlHighVoltage();
  virtual ~ControlHighVoltage();
protected:
  ControlHighVoltage(const ControlHighVoltage& r) = default;

public:  
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_finalize() override;

  void setNextVoltage(double v) { nextVoltage_ = v; }
  void setExec(bool v) { exec_ = v; }
  double CurrentVoltage() { return currentVoltage_; }
  double NextVoltage() { return nextVoltage_; }


private:
  int deviceID_ = 0;
  int channel_ = 0;
  std::string ADManagerName_ = "AnalogDiscoveryManager";
  AnalogDiscoveryManager* ADManager_ = nullptr;
  int sleep_ = 500; // ms
  double currentVoltage_ = 0.0; // volt
  double nextVoltage_ = 0.0; // volt
  bool exec_ = false;
};

} /* namespace gramsballoon */

#endif /* ControlHighVoltage_H */
