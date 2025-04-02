#ifndef GramsBalloon_PassTelemetry_hh
#define GramsBalloon_PassTelemetry_hh 1
#include "DividePacket.hh"
#include "MosquittoManager.hh"
#include "anlnext/BasicModule.hh"
namespace gramsballoon::pgrams {
class MosquittoManager;
class ReceiveStatusFromDAQComputer;
class PassTelemetry: public anlnext::BasicModule {
  DEFINE_ANL_MODULE(PassTelemetry, 1.0);
  ENABLE_PARALLEL_RUN();

public:
  PassTelemetry() = default;
  virtual ~PassTelemetry() = default;

protected:
  PassTelemetry(const PassTelemetry &r) = default;

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;

private:
  std::string topic_ = "Telemetry";
  MosquittoManager *mosquittoManager_ = nullptr;
  std::string mosquittoManagerName_ = "MosquittoManager";
  DividePacket *dividePacket_ = nullptr;
  std::string dividePacketName_ = "DividePacket";
  int chatter_ = 0;
  int qos_ = 0;
};
} // namespace gramsballoon::pgrams
#endif // PassTelemetry_hh