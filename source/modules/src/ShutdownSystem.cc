#include "ShutdownSystem.hh"
#include <string.h>
#include <errno.h>

using namespace anlnext;

namespace gramsballoon {

ANLStatus ShutdownSystem::mod_define()
{
  define_parameter("buffer_time_sec", &mod_class::bufferTimeSec_);
  
  return AS_OK;
}

ANLStatus ShutdownSystem::mod_analyze()
{
  if (prepareReboot_ && reboot_) {
    return AS_QUIT;
  }
  else if (prepareShutdown_ && shutdown_) {
    return AS_QUIT;
  }
  /*
  if (reboot_) {
    clearStatus();
  }
  if (shutdown_) {
    clearStatus();
  }
  if (prepareReboot_ && prepareShutdown_) {
    clearStatus();
  }
  */
  /*
  if (prepareReboot_) {
    timeval now;
    gettimeofday(&now, NULL);
    if (now.tv_sec-prepareRebootTime_.tv_sec>bufferTimeSec_) {
      prepareReboot_ = false;
    }
  }

  if (prepareShutdown_) {
    timeval now;
    gettimeofday(&now, NULL);
    if (now.tv_sec-prepareShutdownTime_.tv_sec>bufferTimeSec_) {
      prepareShutdown_ = false;
    }
  }
  */
    
  return AS_OK;
}

ANLStatus ShutdownSystem::mod_finalize()
{
  int rslt = 0;
  if (prepareReboot_ && reboot_) {
    sync();
    rslt = reboot(LINUX_REBOOT_CMD_RESTART);
  }
  else if (prepareShutdown_ && shutdown_) {
    sync();
    rslt = reboot(LINUX_REBOOT_CMD_HALT);
  }

  if (rslt < 0) {
    std::cerr << "Error in ShutdownSystem::mod_analyze()" << std::endl;
    std::cerr << "Error code: " << strerror(errno) << std::endl;
  }

  return AS_OK;
}

void ShutdownSystem::clearStatus()
{
  setPrepareReboot(false);
  setReboot(false);
  setPrepareShutdown(false);
  setShutdown(false);
}

void ShutdownSystem::setPrepareReboot(bool v)
{
  singleton_self()->prepareReboot_ = v;
  if (v) {
    gettimeofday(&prepareRebootTime_, NULL);
  }
}

void ShutdownSystem::setPrepareShutdown(bool v)
{
  singleton_self()->prepareShutdown_ = v;
  if (v) {
    gettimeofday(&prepareShutdownTime_, NULL);
  }
}


} /* namespace gramsballoon */
