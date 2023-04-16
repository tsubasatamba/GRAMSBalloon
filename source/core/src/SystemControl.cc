#include "SystemControl.hh"

namespace gramsballoon
{
  void SystemControl::reboot_system()
  {
    sync();
    reboot(LINUX_REBOOT_CMD_RESTART);
  }
  void SystemControl::shutdown_system()
  {
    sync();
    reboot(LINUX_REBOOT_CMD_HALT);
  }
  SystemStatus::SystemStatus(const std::string &path)
  {
    path_ = path;
    statfs64(path_.data(), &capacity_);
  }
  SystemStatus::SystemStatus()
  {
    path_ = "/";
    statfs64(path_.data(), &capacity_);
  }
  long long SystemStatus::get_freesize()
  {
    int rslt = statfs64(path_.data(), &capacity_);
    if (rslt < 0)
    {
      std::cerr << "failed in get_freesize(). Code: " << rslt << std::endl;
      return -1;
    }
    return capacity_.f_bfree * capacity_.f_bsize;
  }
  long long SystemStatus::get_allsize()
  {
    int rslt = statfs64(path_.data(), &capacity_);
    if (rslt < 0)
    {
      std::cerr << "failed in get_allsize(). Code: " << rslt << std::endl;
      return -1;
    }
    return capacity_.f_blocks * capacity_.f_bsize;
  }
}