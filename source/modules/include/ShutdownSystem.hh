#ifndef ShutdownSystem_H
#define ShutdownSystem_H 1

/**
 * Test module for shutdown system
 * @author Shota Arai, Tsubasa Tamba
 * @date 2023-04-16
 */

#include <anlnext/BasicModule.hh>
#include <sys/reboot.h>
#include <linux/reboot.h>
#include "SystemControl.hh"

namespace gramsballoon {

class ShutdownSystem : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(ShutdownSystem, 1.0);
  ENABLE_PARALLEL_RUN();

public:
  ShutdownSystem() = default;
  virtual ~ShutdownSystem() = default;

protected:
  ShutdownSystem(const ShutdownSystem &r) = default;

public:
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_finalize() override;
  void setReboot(bool v) { reboot_ = v; }
  void setShutdown(bool v) { shutdown_ = v; }
  
private:
  bool reboot_ = false;
  bool shutdown_ = false;
};

} /* namespace gramsballoon */

#endif /*ShutdownSystem_H*/