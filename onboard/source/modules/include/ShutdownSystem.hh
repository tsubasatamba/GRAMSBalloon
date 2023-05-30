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
#include <sys/time.h>
#include "SendTelemetry.hh"

namespace gramsballoon {

class SendTelemetry;

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
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_finalize() override;
  void setReboot(bool v);
  void setShutdown(bool v);
  void setPrepareReboot(bool v);
  void setPrepareShutdown(bool v);
  void clearStatus();
  SendTelemetry* getSendTelemetry() { return singleton_self()->sendTelemetry_; }
  
private:
  bool reboot_ = false;
  bool shutdown_ = false;
  bool prepareReboot_ = false;
  bool prepareShutdown_ = false;
  int bufferTimeSec_ = 60;
  timeval prepareRebootTime_;
  timeval prepareShutdownTime_;
  SendTelemetry* sendTelemetry_ = nullptr;
  int chatter_ = 0;
};

} /* namespace gramsballoon */

#endif /*ShutdownSystem_H*/
