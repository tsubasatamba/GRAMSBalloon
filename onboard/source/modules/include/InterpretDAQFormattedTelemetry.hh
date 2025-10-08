/**
 * Decode the DAQ formatted telemetry
 * @author Shota Arai
 * @date 2025/04/02 | First version
 */

#ifndef GB_InterpretDAQFormattedTelemetry_hh
#define GB_InterpretDAQFormattedTelemetry_hh 1
#include "CommunicationFormat.hh"
#include "ReceiveTelemetry.hh"
#include "SendTelemetry.hh"
#include "anlnext/BasicModule.hh"
namespace gramsballoon {
class SendTelemetry;
} // namespace gramsballoon
namespace gramsballoon::pgrams {
class ReceiveTelemetry;
class InterpretDAQFormattedTelemetry: public anlnext::BasicModule {
  DEFINE_ANL_MODULE(InterpretDAQFormattedTelemetry, 1.0);
  ENABLE_PARALLEL_RUN();

public:
  InterpretDAQFormattedTelemetry() = default;
  virtual ~InterpretDAQFormattedTelemetry() = default;

protected:
  InterpretDAQFormattedTelemetry(const InterpretDAQFormattedTelemetry &r) = default;
  CommunicationFormat *getCommunicationFormat() const {
    return communicationFormat_;
  }

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  virtual void printTelemetry() const;

private:
  ReceiveTelemetry *receiveTelemetry_ = nullptr;
  SendTelemetry *sendTelemetry_ = nullptr;
  CommunicationFormat *communicationFormat_ = nullptr;
  std::string receiveTelemetryName_ = "ReceiveTelemetry";
  int chatter_ = 0;
  bool printTelemetry_ = false;
};
} // namespace gramsballoon::pgrams
#endif // GB_InterpretDAQTelemetry_hh