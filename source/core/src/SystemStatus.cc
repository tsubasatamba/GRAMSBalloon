#include "SystemControl.hh"

namespace gramsballoon {
int SystemControl::rebootSystem()
{
  sync();
  return reboot(LINUX_REBOOT_CMD_RESTART);
}

int SystemControl::shutdownSystem()
{
  sync();
  return reboot(LINUX_REBOOT_CMD_HALT);
}

/*
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
*/
/*
long long SystemStatus::getFreesize()
{
  int rslt = statfs64(path_.data(), &capacity_);
  if (rslt < 0) {
    std::cerr << "failed in get_freesize(). Code: " << rslt << std::endl;
    return -1;
  }
  return capacity_.f_bfree * capacity_.f_bsize;
}

long long SystemStatus::getAllsize()
{
  int rslt = statfs64(path_.data(), &capacity_);
  if (rslt < 0) {
    std::cerr << "failed in get_allsize(). Code: " << rslt << std::endl;
    return -1;
  }
  return capacity_.f_blocks * capacity_.f_bsize;
}
*/

} /* namespace gramsballoon*/
