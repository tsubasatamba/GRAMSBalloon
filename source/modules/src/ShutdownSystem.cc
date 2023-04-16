#include "ShutdownSystem.hh"
#include <string.h>
#include <errno.h>

using namespace anlnext;
namespace gramsballoon
{

  ANLStatus ShutdownSystem::mod_define()
  {
    define_parameter("confirmed", &mod_class::confirmed_);
    define_parameter("reboot", &mod_class::reboot_);
    return AS_OK;
  }
  ANLStatus ShutdownSystem::mod_initialize()
  {
    return AS_OK;
  }
  ANLStatus ShutdownSystem::mod_finalize() /*this could be changed to mod_analyze*/
  {
    int rslt = 0;
    if (confirmed_)
    {
      if (reboot_)
      {
        rslt = SystemControl::reboot_system();
      }
      else
      {
        rslt = SystemControl::shutdown_system();
      }
    }
    if (rslt < 0)
    {
#if 1
      std::cerr << "Error code: " << strerror(errno) << std::endl;
#endif
      return AS_ERROR;
    }
    return AS_OK;
  }

} /*namespace gramsballoon*/