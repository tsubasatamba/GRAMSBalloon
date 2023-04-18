/**
 * Classes to control Linux system and get status about the system
 * @authors Tsubasa Tamba, Shota Arai
 * @date 2023-04-16
 */
#ifndef SystemControl_H
#define SystemControl_H 1
#include <sys/reboot.h>
#include <linux/reboot.h>
#include <unistd.h>
#include <sys/vfs.h>
#include <sys/types.h>
#include <string>
#include <memory>
#include <iostream>

namespace gramsballoon {
/*
class SystemStatus
{
public:
  SystemStatus(const std::string &path);
  SystemStatus();
  ~SystemStatus() = default;
  long long getFreesize();
  long long getAllsize();

protected:
  SystemStatus(const SystemStatus &r) = default;

private:
  struct statfs64 capacity_;
  std::string path_;
};
*/

class SystemControl
{
public:
  SystemControl() = default;
  ~SystemControl() = default;
  
protected:
  SystemControl(const SystemControl &r) = default;
public:
  static int rebootSystem();
  static int shutdownSystem();
};

} /*namespace gramsballoon*/

#endif /*SystemControl_H*/
