#ifndef InterpretTelemetry_H
#define InterpretTelemetry_H 1

#include <anlnext/BasicModule.hh>
#include <thread>
#include <chrono>
#include "TelemetryDefinition.hh"
#include "ReceiveTelemetry.hh"

namespace gramsballoon {

class InterpretTelemetry : public anlnext::BasicModule {
  DEFINE_ANL_MODULE(InterpretTelemetry, 1.0);

public:
  InterpretTelemetry();
  virtual ~InterpretTelemetry();

protected:
  InterpretTelemetry(const InterpretTelemetry &r) = default;
  
public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_finalize() override;

  TelemetryDefinition* Telemdef() { return (singleton_self()->telemdef_).get(); }
  int CurrentTelemetryType() { return singleton_self()->currentTelemetryType_; }
  
private:
  std::shared_ptr<TelemetryDefinition> telemdef_;
  ReceiveTelemetry* receiver_;
  int currentTelemetryType_ = 0;

};

} // namespace gramsballoon
#endif // InterpretTelemetry_H
