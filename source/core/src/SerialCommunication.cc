#include "SerialCommunication.hh"

namespace gramsballoon {

SerialCommunication::SerialCommunication()
  :baudrate_(B9600), openMode_(O_RDWR | O_NONBLOCK)
{
  serialPath_ = "/dev/null";
  tio_ = std::make_unique<termios>();
}

SerialCommunication::SerialCommunication(const std::string& serial_path, speed_t baudrate, mode_t open_mode)
{
  tio_ = std::make_unique<termios>();
  serialPath_ = serial_path;
  baudrate_ = baudrate;
  openMode_ = open_mode;
}

SerialCommunication::~SerialCommunication()
{
  close(fd_);
}

int SerialCommunication::initialize()
{
  cfsetospeed(tio_.get(), baudrate_);
  cfsetispeed(tio_.get(), baudrate_);
  std::cout << "BAUDRATE was set to " << baudrate_ << std::endl;
  
  cfmakeraw(tio_.get());
  std::cerr << "Set to raw mode" << std::endl;
  
  fd_ = open(serialPath_.c_str(), O_RDWR | O_NONBLOCK);
  if (fd_ < 0) {
    std::cout << "Open Error" << std::endl;
    return -1;
  }
  std::cout << "Open Serial port" << std::endl;
  tio_->c_cflag |= CREAD;
  tio_->c_cflag |= CS8;
  tio_->c_cflag |= CLOCAL;
  tio_->c_cflag |= PARENB;
  //tio_->c_cflag |= CSTOPB;

  int status = tcsetattr(fd_, TCSANOW, tio_.get());
  if (status!=0) {
    std::cerr << "tcsetattr failed" << std::endl;
    return -1;
  }
  
  status = ioctl(fd_, TCSETS, tio_.get());
  if (status!=0) {
    std::cout << "ioctl failed" << std::endl;
    return -1;
  }
  status = fcntl(fd_, F_SETFL, openMode_);
  if (status!=0) {
    std::cout << "fcntl failed" << std::endl;
    return -1;
  }
  
  return 0;
}

int SerialCommunication::sreadSingle(uint8_t& buf)
{
  const int length = 1;
  const int status = read(fd_, &buf, length);
  return status;
}

int SerialCommunication::sread(std::vector<uint8_t>& buf, int length)
{
  const int status = read(fd_, &buf[0], length);
  return status;
}

int SerialCommunication::swrite(const std::vector<uint8_t>& buf)
{
  const int length = buf.size();
  const int status = write(fd_, &buf[0], length);
  
  return status;
}

} /* namespace gramsballoon */


