#ifndef GRAMS_BALLOON_Telemetry_H
#define GRAMS_BALLOON_Telemetry_H 1

#define TEL_SERIAL_PORT "/dev/serial0"
#define TEL_BAUDRATE B57600

#include <anlnext/BasicModule.hh>
#include <vector>
#include <termios.h>

class Telemetry : public anlnext::BasicModule
{
    DEFINE_ANL_MODULE(Telemetry, 1.0)
public:
    telemetry();
    ~telemetry();
    anlnext::ANLStatus mod_define() override;
    anlnext::ANLStatus mod_initialize() override;
    anlnext::ANLStatus mod_analyze() override;
    anlnext::ANLStatus mod_finalize() override;
    float recv_daq;
    float send_daq;
    std::vector<int> recv_env;
    int send_env;
    std::vector<int> recv_cham;
    int send_cham;

protected:
    std::string filename_;
    int buff_size = 256;
    char *buffer;
    struct termios tel_tio;
    int fd;
};

telemetry::telemetry()
{
}

telemetry::~telemetry()
{
}

#endif