/**
 * Classes to control Linux system and get status about the system
 * @authors Tsubasa Tamba, Shota Arai
 * @date 2023-04-16s
 */
#ifndef SystemControl_HH
#define SystemControl_HH 1
#include <sys/reboot.h>
#include <linux/reboot.h>
#include <unistd.h>
#include <sys/vfs.h>
#include <sys/types.h>
#include <string>
#include <memory>
#include <iostream>

namespace gramsballoon
{
  class SystemStatus
  {
  public:
    SystemStatus(const std::string &path);
    SystemStatus();
    ~SystemStatus() = default;
    long long get_freesize();
    long long get_allsize();

  protected:
    SystemStatus(const SystemStatus &r) = default;

  private:
    struct statfs64 capacity_;
    std::string path_;
  };

  class SystemControl
  {
  public:
    SystemControl() = default;
    ~SystemControl() = default;

  protected:
    SystemControl(const SystemControl &r) = default;
    static void reboot_system();
    static void shutdown_system();
  };
} /*namespace gramsballoon*/
#endif /*SystemControl_HH*/