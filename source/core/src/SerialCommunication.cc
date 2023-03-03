#include "SerialCommunication.hh"


SerialCommunication::SerialCommunication()
  :inputBaudrate_(B1200), outputBaudrate_(B57600), openMode_(O_RDWR)
{
  serialPath_ = "/dev/null";
  tio_ = std::make_unique<termios>();
}

SerialCommunication::SerialCommunication(const std::string& serial_path, speed_t input_baudrate, speed_t output_baudrate, int open_mode)
{
  tio_ = std::make_unique<termios>();
  serialPath_ = serial_path;
  inputBaudrate_ = input_baudrate;
  outputBaudrate_ = output_baudrate;
  openMode_ = open_mode;
}

SerialCommunication::~SerialCommunication()
{
  close(fd_);
}


void SerialCommunication::setBaudrate(speed_t v1, speed_t v2)
{
  inputBaudrate_ = v1;
  outputBaudrate_ = v2;
}


int SerialCommunication::initialize()
{
  cfsetospeed(tio_.get(), inputBaudrate_);
  cfsetispeed(tio_.get(), inputBaudrate_);
  std::cout << "INPUT BAUDRATE was set to " << inputBaudrate_ << std::endl;
  std::cout << "OUTPUT BAUDRATE was set to " << outputBaudrate_ << std::endl;
  
  cfmakeraw(tio_.get());
  std::cerr << "Set to raw mode" << std::endl;
  
  fd_ = open(serialPath_.c_str(), openMode_);
  if (fd_ < 0) {
    std::cout << "Open Error" << std::endl;
    return -1;
  }
  std::cout << "Open Serial port\n";
  tio_->c_cflag |= CREAD;
  tio_->c_cflag |= CS8;
  tio_->c_cflag |= CLOCAL;
  tio_->c_cflag |= PARENB;
  tio_->c_cflag |= CSTOPB;

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
  return 0;
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




