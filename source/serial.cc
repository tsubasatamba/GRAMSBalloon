#include "serial.hh"

int serial::initialize()
{
    cfsetospeed(&tio, baudrate);
    std::cout << "BAUDRATE was set to " << baudrate << "\n";
    cfsetispeed(&tio, baudrate);
    cfmakeraw(&tio);
    std::cout << "Set to raw mode\n";
    fd = open(serialpath.c_str(), mode);
    if (fd < 0)
    {
        std::cout << "Open Error" << std::endl;
        return -1;
    }
    std::cout << "Open Serial port\n";
    tio.c_cflag += CREAD;
    tio.c_cflag += CS8;
    tio.c_cflag += CLOCAL;
    tio.c_cflag += PARENB;
    tio.c_cflag += CSTOPB;
    tcsetattr(fd, TCSANOW, &tio);
    ioctl(fd, TCSETS, &tio);
    return 0;
}
int serial::swrite(std::string *buf)
{
    const char *temp = buf->c_str();
    return write(this->fd, temp, (int)(buf->length()));
}
int serial::sread(std::string *buf)
{
    char *temp;
    return read(this->fd, temp, (int)(buf->length()));
}
