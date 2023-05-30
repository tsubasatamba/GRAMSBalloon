#include "ShutdownSystem.hh"
#include <string.h>
#include <errno.h>

using namespace anlnext;

namespace gramsballoon {

ANLStatus ShutdownSystem::mod_define()
{
  define_parameter("buffer_time_sec", &mod_class::bufferTimeSec_);
  define_parameter("chatter", &mod_class::chatter_);
  
  return AS_OK;
}

ANLStatus ShutdownSystem::mod_initialize()
{
  const std::string send_telem_md = "SendTelemetry";
  if (exist_module(send_telem_md)) {
    get_module_NC(send_telem_md, &sendTelemetry_);
  }

  gettimeofday(&prepareRebootTime_, NULL);
  gettimeofday(&prepareShutdownTime_, NULL);
  
  return AS_OK;
}

ANLStatus ShutdownSystem::mod_analyze()
{
  if (reboot_) {
    return AS_QUIT_ALL;
  }
  else if (shutdown_) {
    return AS_QUIT_ALL;
  }
    
  if (prepareReboot_ && prepareShutdown_) {
    clearStatus();
    return AS_OK;
  }
  
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
    
  return AS_OK;
}

ANLStatus ShutdownSystem::mod_finalize()
{
  int rslt = 0;
  if (reboot_) {
    sync();
    rslt = reboot(LINUX_REBOOT_CMD_RESTART);
  }
  else if (shutdown_) {
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
    gettimeofday(&(singleton_self()->prepareRebootTime_), NULL);
  }
}

void ShutdownSystem::setPrepareShutdown(bool v)
{
  singleton_self()->prepareShutdown_ = v;
  if (v) {
    gettimeofday(&(singleton_self()->prepareShutdownTime_), NULL);
  }
}

void ShutdownSystem::setReboot(bool v)
{
  if (singleton_self()->prepareReboot_) {
    singleton_self()->reboot_ = v;
  }
  else {
    if (getSendTelemetry()) {
      std::cout << "set reboot rejected \n\n\n\n\n\n\n\n\n" << std::endl;
      getSendTelemetry()->getErrorManager()->setError(ErrorType::REBOOT_REJECTED);
    }
  }
}

void ShutdownSystem::setShutdown(bool v)
{
  if (singleton_self()->prepareShutdown_) {
    singleton_self()->shutdown_ = v;
  }
  else {
    if (getSendTelemetry()) {
      std::cout << "set shutdown rejected \n\n\n\n\n\n\n\n\n" << std::endl;
      getSendTelemetry()->getErrorManager()->setError(ErrorType::SHUTDOWN_REJECTED);
    }
  }
}


} /* namespace gramsballoon */
