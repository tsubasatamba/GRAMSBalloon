#ifndef ReceiveTelemetry_H
#define ReceiveTelemetry_H 1

#include <anlnext/BasicModule.hh>
#include "SerialCommunication.hh"
#include <queue>
#include <TelemetryDefinition.hh>
#include <thread>
#include <chrono>

namespace gramsballoon
{
    class ReceiveTelemetry : public anlnext::BasicModule
    {
        DEFINE_ANL_MODULE(ReceiveTelemetry, 1.0);

    public:
        ReceiveTelemetry();
        virtual ~ReceiveTelemetry();

    protected:
        ReceiveTelemetry(const ReceiveTelemetry &r) = default;

    public:
        anlnext::ANLStatus mod_define() override;
        anlnext::ANLStatus mod_initialize() override;
        anlnext::ANLStatus mod_analyze() override;
        anlnext::ANLStatus mod_finalize() override;

    private:
        std::vector<uint8_t> telemetry_;
        std::queue<uint8_t> que_;
        std::shared_ptr<TelemetryDefinition> teldef_ = nullptr;
        int maxTelemetry_ = 32000;

        // communication
        std::shared_ptr<SerialCommunication> sc_ = nullptr;
        speed_t baudrate_;
        std::string serialpath_;
        char openMode_ = O_RDWR;
        bool startReading_ = false;
    };
} // namespace gramsballoon
#endif // ReceiveTelemetry_H