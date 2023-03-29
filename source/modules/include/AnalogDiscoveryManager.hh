/**
 * AnalogDiscovery Manager
 *
 * @author Tsubasa Tamba, Shota Arai
 * @date 2023-03-15
 */


#ifndef AnalogDiscoveryManager_H
#define AnalogDiscoveryManager_H 1

#include <anlnext/BasicModule.hh>
#include "AnalogDiscoveryIO.hh"

namespace gramsballoon{

class AnalogDiscoveryManager : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(AnalogDiscoveryManager, 1.0);
  ENABLE_PARALLEL_RUN();

public:
  AnalogDiscoveryManager();
  virtual ~AnalogDiscoveryManager();
protected:
  AnalogDiscoveryManager(const AnalogDiscoveryManager& r) = default;

public:  
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_finalize() override;

  AnalogDiscoveryIO* ADIO() { return ADIO_.get(); }


private:
  std::shared_ptr<AnalogDiscoveryIO> ADIO_ = nullptr;
};

} /* namespace gramsballoon */

#endif /* AnalogDiscoveryManager_H */
