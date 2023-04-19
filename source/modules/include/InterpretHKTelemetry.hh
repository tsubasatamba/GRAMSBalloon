#ifndef InterpretHKTelemetry_H
#define InterpretHKTelemetry_H 1

#include <anlnext/BasicModule.hh>
#include <thread>
#include <chrono>
#include "TelemetryDefinition.hh"
#include "ReceiveTelemetry.hh"

namespace gramsballoon {

class InterpretHKTelemetry : public anlnext::BasicModule {
  DEFINE_ANL_MODULE(InterpretHKTelemetry, 1.0);

public:
  InterpretHKTelemetry();
  virtual ~InterpretHKTelemetry();

protected:
  InterpretHKTelemetry(const InterpretHKTelemetry &r) = default;
  
public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_finalize() override;

  
private:
  std::shared_ptr<TelemetryDefinition> telemdef_;
  ReceiveTelemetry* receiver_;

};

} // namespace gramsballoon
#endif // InterpretHKTelemetry_H
