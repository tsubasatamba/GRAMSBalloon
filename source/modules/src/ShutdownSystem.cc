#include "ShutdownSystem.hh"
#include <string.h>
#include <errno.h>

using namespace anlnext;

namespace gramsballoon {

ANLStatus ShutdownSystem::mod_analyze()
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
    return AS_QUIT_ERROR;
  }
  
  return AS_OK;
}

ANLStatus ShutdownSystem::mod_finalize()
{
  return AS_OK;
}

} /* namespace gramsballoon */
