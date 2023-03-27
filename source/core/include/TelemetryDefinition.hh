#ifndef TelemetryDefinition_H
#define TelemetryDefinition_H 1


#include <iostream>
#include <vector>
#include <sys/time.h>
#include <openssl/md5.h>

#define TELEM_TYPE_NORMAL 1
#define TELEM_TYPE_WAVE 2

/**
 * A class to define telemetry
 *
 * @author Tsubasa Tamba, Shota Arai
 * @date 2023-03-23
 */

namespace GRAMSBalloon {

class TelemetryDefinition
{
public:
  TelemetryDefinition();
  void generateTelemetry(int telem_type);
  void generateTelemetryNormal();
  void generateTelemetryWave();
  void writeRTDTemperature();
  void writeEnvironmentalData();
  void writeMD5();
  void clear();

  
  template<typename T> void addValue(T input);
  template<typename T> void addVector(std::vector<T>& input);

  

  const std::vector<uint8_t>& Telemetry() const { return telemetry_; }
  std::vector<double>& EnvTemperature() { return envTemperature_; }
  std::vector<double>& EnvHumidity() { return envHumidity_; }
  std::vector<double>& EnvPressure() { return envPressure_; }
  
  
private:
  std::vector<uint8_t> telemetry_;
  timeval time_now;
  uint32_t telemIndex_ = 0;
  std::vector<double> envTemperature_;
  std::vector<double> envHumidity_;
  std::vector<double> envPressure_;

 
};

} /* namespace GRAMSBalloon */

#endif /* TelemetryDefinition_H */
