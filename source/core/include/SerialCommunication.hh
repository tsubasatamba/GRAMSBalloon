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


class SerialCommunication
{
public:
  SerialCommunication();
  SerialCommunication(const std::string& serial_path, speed_t input_baudrate, speed_t output_baudrate, int open_mode);
  ~SerialCommunication();
  int initialize();
  int sread(std::vector<uint8_t>& buf, int length);
  int swrite(const std::vector<uint8_t>& buf);
  void setBaudrate(speed_t v1, speed_t v2);

  void setInputBaudrate(speed_t v) { inputBaudrate_ = v; }
  void setOutputBaudrate(speed_t v) { outputBaudrate_ = v; }
  void setSerialPath(const std::string& s) { serialPath_ = s; }
  void setOpenMode(char c) { openMode_ = c; }

  
private:
  std::unique_ptr<termios> tio_ = nullptr;
  int fd_ = 0;
  speed_t inputBaudrate_ = 0;
  speed_t outputBaudrate_ = 0;
  std::string serialPath_;
  char openMode_;
};



#endif /* SerialCommunication_H */
