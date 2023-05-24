#include <vector>
#include <iostream>
#include <fstream>
#include <cstdint>

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


int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    std::cout << "Usage: send_command <filename>" << std::endl;
    return 0;
  }

  std::cout << SERIAL_PORT << std::endl;

  struct termios tio;
  int fd;
  std::ifstream ifs(argv[1]);
  std::string temp;
  ifs >> temp;

  std::cout << "Read command: " << temp << std::endl;

  fd = open(SERIAL_PORT, O_RDWR | O_NONBLOCK);
  if (fd < 0)
  {
    std::cout << "Open Error\n";
    return -1;
  }
  std::cout << "opened" << std::endl;
  tio.c_cflag |= CREAD;
  tio.c_cflag |= CLOCAL;
  tio.c_cflag |= CS8;
  tio.c_cflag |= PARENB;
  tio.c_cflag |= CSTOPB;
  cfsetispeed(&tio, BAUDRATE);
  cfsetospeed(&tio, BAUDRATE);
  cfmakeraw(&tio);
  tcsetattr(fd, TCSANOW, &tio);
  ioctl(fd, TCSETS, &tio);
  fcntl(fd, F_SETFL, O_RDWR); // reference: https://github.com/orbcode/orbuculum/issues/15

  std::vector<uint8_t> buf;
  uint8_t value;
  for (int i = 0; i < temp.size(); i += 2)
  {
    value = static_cast<uint8_t>(std::stoi(temp.substr(i, 2), nullptr, 16));
    std::cout << static_cast<int>(value) << std::endl;
    buf.push_back(value);
  }
  std::cout << "buf_size: " << buf.size() << std::endl;
  std::cout << "hex: ";
  for (int i = 0; i < buf.size(); i++) {
    std::cout << static_cast<int>(buf[i]) << " ";
  }
  std::cout << std::endl;

  const int status = write(fd, &buf[0], buf.size());

  std::cout << status << std::endl;
  std::cout << errno << std::endl;

  close(fd);

  return 0;
}
