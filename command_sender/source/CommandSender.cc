#include "CommandSender.hh"

#include <cstdint>
#include <vector>
#include <iostream>

#include <sys/ioctl.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>

#define BAUDRATE B1200

#ifdef __APPLE__
#define TCSETS TIOCSETA
#define SERIAL_PORT "/dev/tty.usbserial-1130"
#else
#define SERIAL_PORT "/dev/ttyAMA0"
#endif

namespace gramsballoon
{

CommandSender::CommandSender()
{
  serial_port_ = SERIAL_PORT;
}

bool CommandSender::open_serial_port()
{
  std::cout << "CommandSender: opening " << serial_port_ << std::endl;

  int fd = open(serial_port_.c_str(), O_RDWR | O_NONBLOCK);
  if (fd < 0) {
    std::cout << "CommandSender: open error" << std::endl;
    return false;
  }

  cfmakeraw(&tio_);
  
  tio_.c_cflag |= CREAD;
  tio_.c_cflag |= CLOCAL;
  tio_.c_cflag |= CS8;
  //tio_.c_cflag |= PARENB;
  tio_.c_cflag &= (~PARENB);
  //tio_.c_cflag |= CSTOPB;
  tio_.c_cflag &= (~CSTOPB);
  cfsetispeed(&tio_, BAUDRATE);
  cfsetospeed(&tio_, BAUDRATE);
  
  tcsetattr(fd, TCSANOW, &tio_);
  ioctl(fd, TCSETS, &tio_);
  fcntl(fd, F_SETFL, O_RDWR); // reference: https://github.com/orbcode/orbuculum/issues/15

  fd_ = fd;

  return true;
}

void CommandSender::close_serial_port()
{
  close(fd_);
}

int CommandSender::send(const std::vector<uint8_t>& byte_array)
{
  const int rval = write(fd_, &byte_array[0], byte_array.size());
  return rval;
}

} /* namespace gramsballoon */
