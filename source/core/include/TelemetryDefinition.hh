#ifndef TelemetryDefinition_H
#define TelemetryDefinition_H 1


#include <iostream>
#include <vector>
#include <sys/time.h>
#include <openssl/md5.h>


/**
 * A class to define telemetry
 *
 * @author Tsubasa Tamba, Shota Arai
 * @date 2023-03-23
 */

namespace GRAMSBalloon {

enum class TelemetryType {
  normal = 1,
  wave = 2,
  status = 3
};

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


  void setEnvTemperature(int index, double v) { envTemperature_[index] = v; }
  void setEnvHumidity(int index, double v) { envTemperature_[index] = v; }
  void setEnvPressure(int index, double v) { envPressure_[index] = v; }
  void resizeEnvTemperature(int n) { envTemperature_.resize(n); }
  void resizeEnvHumidity(int n) { envHumidity_.resize(n); }
  void resizeEnvPressure(int n) { envPressure_.resize(n); }
  std::vector<double>& EnvTemperature() { return envTemperature_; }
  std::vector<double>& EnvHumidity() { return envHumidity_; }
  std::vector<double>& EnvPressure() { return envPressure_; }

  void setEventID(int v) { eventID_ = v; }
  void setEventHeader(const std::vector<short>& v) { eventHeader_ = v; }
  void setEventData(const std::vector<std::vector<short>>& v) { eventData_ = v; }
  int EventID() { return eventID_; }
  const std::vector<short>& EventHeader() const { return eventHeader_; }
  const std::vector<std::vector<short>>& EventData() const { return eventData_; }

  void setLastCommandCode(uint16_t v) { lastCommandCode_ = v; }

  void setRTDTemperatureADC(int index, int16_t v) { RTDTemperatureADC_[index] = v; }
  void resizeRTDTemperatureADC(int n) { RTDTemperatureADC_.resize(n); }
  std::vector<int16_t>& RTDTemperatureADC() { return RTDTemperatureADC_; }
  
private:
  std::vector<uint8_t> telemetry_;
  timeval time_now;
  uint32_t telemIndex_ = 0;
  // info
  std::vector<double> envTemperature_;
  std::vector<double> envHumidity_;
  std::vector<double> envPressure_;
  int eventID_;
  std::vector<short> eventHeader_;
  std::vector<std::vector<short>> eventData_;
  std::vector<int16_t> RTDTemperatureADC_;
  uint16_t lastCommandCode_;
};

} /* namespace GRAMSBalloon */

#endif /* TelemetryDefinition_H */
