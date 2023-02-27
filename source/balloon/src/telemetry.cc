#include "telemetry.hh"
#include <pigpio.h>
#include <termios.h>
#include "serial.hh"
#include <iostream>

using namespace anlnext;

Telemetry::telemetry()
{
}
Telemetry::~telemetry() = default;

ANLStatus Telemetry::mod_define()
{
    define_parameter("filename", &mod_class::filename_);
    return AS_OK;
}

ANLStatus Telemetry::mod_initialize()
{
    serial tel_serial = 
    buffer = new char[buff_size];
    return AS_OK;
}

ANLStatus Telemetry::mod_analyze()
{
    return AS_OK;
}

ANLStatus Telemetry::mod_finalize()
{
    delete[] buffer;
    return AS_OK;
}
