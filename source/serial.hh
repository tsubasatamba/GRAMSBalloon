#include <termios.h>
#include <string>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string>
#include <unistd.h>
#include <iostream>
#include <termios.h>

#ifdef __APPLE__
#define TCSETS TIOCSETA
#endif

#ifndef SERIAL_H
#define SERIAL_H 1
#endif

class serial
{
private:
    struct termios tio;
    std::string serialpath;
    speed_t baudrate;
    int fd;
    std::string buffer;
    char mode;

public:
    serial(const std::string serial_path, speed_t baudrate, const int open_mode);
    int initialize();
    int swrite(std::string *buf);
    int sread(std::string *buf);
    ~serial();
};

serial::serial(const std::string serial_path, speed_t baudrate, const int open_mode)
{
    serial::serialpath = serial_path;
    serial::baudrate = baudrate;
    serial::mode = open_mode;
}

serial::~serial()
{
    close(fd);
}
