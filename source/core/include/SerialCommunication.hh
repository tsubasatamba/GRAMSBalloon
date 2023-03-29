#ifndef SerialCommunication_H
#define SerialCommunication_H 1

/**
 * A class of serial communication
 *
 * @author Tsubasa Tamba, Shota Arai
 * @date 2023-03-02
 */

#include "termios.h"
#include <string>
#include <iostream>
#include <memory>
#include "unistd.h"
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <vector>

namespace gramsballoon {

class SerialCommunication
{
public:
  SerialCommunication();
  SerialCommunication(const std::string& serial_path, speed_t baudrate, int open_mode);
  ~SerialCommunication();
  int initialize();
  int sreadSingle(uint8_t& buf);
  int sread(std::vector<uint8_t>& buf, int length);
  int swrite(const std::vector<uint8_t>& buf);

  void setBaudrate(speed_t v) { baudrate_ = v; }
  void setSerialPath(const std::string& s) { serialPath_ = s; }
  void setOpenMode(char c) { openMode_ = c; }

  int FD() { return fd_; }

  
private:
  std::unique_ptr<termios> tio_ = nullptr;
  int fd_ = 0;
  speed_t baudrate_ = B9600;
  std::string serialPath_;
  char openMode_;
};

} /* namespace gramsballoon */

#endif /* SerialCommunication_H */
