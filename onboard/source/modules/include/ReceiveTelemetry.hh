#ifndef ReceiveTelemetry_H
#define ReceiveTelemetry_H 1

#include <anlnext/BasicModule.hh>
#include "SerialCommunication.hh"
#include <thread>
#include <chrono>

namespace gramsballoon {

class ReceiveTelemetry : public anlnext::BasicModule {
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

  const std::vector<uint8_t>& Telemetry() const { return telemetry_; }
  std::vector<uint8_t>& Telemetry() { return telemetry_; }
  bool Valid() { return valid_; }
  void setValid(bool v) { valid_ = v;}
  int Chatter() { return chatter_; }
  
private:
  std::vector<uint8_t> telemetry_;
  int maxTelemetry_ = 32000;
  std::vector<uint8_t> buffer_;
  bool valid_;
  
  // communication
  std::shared_ptr<SerialCommunication> sc_ = nullptr;
  speed_t baudrate_;
  std::string serialPath_;
  mode_t openMode_ = O_RDWR;
  int chatter_ = 0;
};

} // namespace gramsballoon
#endif // ReceiveTelemetry_H
