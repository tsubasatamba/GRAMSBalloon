#ifndef GB_InterpretDAQTelemetry_hh
#define GB_InterpretDAQTelemetry_hh 1
#include "CommandDefinition.hh"
#include "ReceiveTelemetry.hh"
#include "anlnext/BasicModule.hh"
namespace gramsballoon::pgrams {
class ReceiveTelemetry;
class InterpretDAQTelemetry: public anlnext::BasicModule {
  DEFINE_ANL_MODULE(InterpretDAQTelemetry, 1.0);
  ENABLE_PARALLEL_RUN();

public:
  InterpretDAQTelemetry() = default;
  virtual ~InterpretDAQTelemetry() = default;

protected:
  InterpretDAQTelemetry(const InterpretDAQTelemetry &r) = default;

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_pre_initialize() override;
  anlnext::ANLStatus mod_begin_run() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_end_run() override;
  anlnext::ANLStatus mod_finalize() override;

private:
};
} // namespace gramsballoon::pgrams
#endif // GB_InterpretDAQTelemetry_hh