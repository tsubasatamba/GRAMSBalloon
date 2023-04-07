#include "ReceiveTelemetry.hh"
using namespace anlnext;

namespace gramsballoon
{
    ReceiveTelemetry::ReceiveTelemetry()
        : baudrate_(B9600), openMode_(O_RDWR | O_NONBLOCK), maxTelemetry_(32000)
    {
        serialpath_ = "/dev/null";
        for (int i = 0; i < 2; i++)
        {
            que_.push(0);
        }
        teldef_ = std::make_unique<TelemetryDefinition>();
    }

    ReceiveTelemetry::~ReceiveTelemetry() = default;
    ANLStatus ReceiveTelemetry::mod_define()
    {
        define_parameter("baudrate", &mod_class::baudrate_);
        define_parameter("serial_path", &mod_class::serialpath_);
        define_parameter("open_mode", &mod_class::openMode_);

        return AS_OK;
    }

    ANLStatus ReceiveTelemetry::mod_initialize()
    {
        sc_ = std::make_unique<SerialCommunication>(serialpath_, baudrate_, openMode_);
        sc_->initialize();

        return AS_OK;
    }
    ANLStatus ReceiveTelemetry::mod_analyze()
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));

        telemetry_.clear();

        fd_set fdset;
        timeval timeout;
        timeout.tv_sec = 10;
        timeout.tv_usec = 0;
        FD_ZERO(&fdset);
        FD_SET(sc_->FD(), &fdset);
        int rv = select(sc_->FD() + 1, &fdset, NULL, NULL, &timeout);
        if (rv == -1)
        {
            std::cerr << "Error in ReceiveTelemetry::mod_analyze: rv = -1" << std::endl;
            return AS_ERROR;
        }

        if (rv == 0)
        {
            std::cout << "Time out" << std::endl;
            return AS_OK;
        }

        std::vector<uint8_t> buffer(maxTelemetry_);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        const int status = sc_->sread(buffer, maxTelemetry_);
        if (status == -1)
        {
            std::cerr << "Read command failed in ReceiveTelemetry::mod_analyze: status = " << status << std::endl;
            return AS_OK;
        }
        std::cout << "status: " << status << std::endl;
        for (int i = 0; i < status; i++)
        {
            telemetry_.push_back(buffer[i]);
        }
#if 0
        for (int i = 0; i < status;i++){
            std::cout << "telemetry[" <<i<< "] = "<<static_cast<int>(telemetry_[i]) << std::endl;
        }
#endif
    }

    ANLStatus ReceiveTelemetry::mod_finalize()
    {
        return AS_OK;
    }

} // namespace gramsballoon