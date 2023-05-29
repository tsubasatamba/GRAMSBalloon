#ifndef GRAMSBALOON_COMMANDSENDER_H
#define GRAMSBALOON_COMMANDSENDER_H 1

#include <cstdint>
#include <vector>
#include <string>
#include <termios.h>

namespace gramsballoon
{

class CommandSender
{
public:
  CommandSender();
  ~CommandSender() = default;

  void set_serial_port(const std::string& dev) { serial_port_ = dev; }
  std::string serial_port() const { return serial_port_; }
  bool open_serial_port();
  void close_serial_port();
  int send(const std::vector<uint8_t>& byte_array);

private:
  std::string serial_port_;
  int fd_ = 0;
  struct termios tio_;
};

} /* namespace gramsballoon */

#endif /* GRAMSBALOON_COMMANDSENDER_H */
