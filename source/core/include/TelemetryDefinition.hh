#ifndef TelemetryDefinition_H
#define TelemetryDefinition_H 1


#include <iostream>
#include <vector>
#include <sys/time.h>
#include "CRC16.hh"


/**
 * A class to define telemetry
 *
 * @author Tsubasa Tamba, Shota Arai
 * @date 2023-03-23
 */

namespace gramsballoon {

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
  void generateTelemetryStatus();
  void writeRTDTemperature();
  void writeEnvironmentalData();
  void writeCRC16();
  void clear();

  void interpret();
  
  template<typename T> void addValue(T input);
  template<typename T> void addVector(std::vector<T>& input);
  template<typename T> T getValue(int index);
  template<typename T> void getVector(int index, int num, std::vector<T>& vec);

  const std::vector<uint8_t>& Telemetry() const { return telemetry_; }

  // detector
  void setEventCount(uint32_t v) { eventCount_ = v; } 

  // env data
  void setEnvTemperature(int index, double v) { envTemperature_[index] = v; }
  void setEnvHumidity(int index, double v) { envTemperature_[index] = v; }
  void setEnvPressure(int index, double v) { envPressure_[index] = v; }
  void resizeEnvTemperature(int n) { envTemperature_.resize(n); }
  void resizeEnvHumidity(int n) { envHumidity_.resize(n); }
  void resizeEnvPressure(int n) { envPressure_.resize(n); }
  std::vector<double>& EnvTemperature() { return envTemperature_; }
  std::vector<double>& EnvHumidity() { return envHumidity_; }
  std::vector<double>& EnvPressure() { return envPressure_; }

  // daq
  void setEventID(uint32_t v) { eventID_ = v; }
  void setEventHeader(const std::vector<int16_t>& v) { eventHeader_ = v; }
  void setEventData(const std::vector<std::vector<int16_t>>& v) { eventData_ = v; }
  int EventID() { return eventID_; }
  const std::vector<int16_t>& EventHeader() const { return eventHeader_; }
  const std::vector<std::vector<int16_t>>& EventData() const { return eventData_; }

  // rtd
  void setRTDTemperatureADC(int index, int16_t v) { RTDTemperatureADC_[index] = v; }
  void resizeRTDTemperatureADC(int n) { RTDTemperatureADC_.resize(n); }
  std::vector<int16_t>& RTDTemperatureADC() { return RTDTemperatureADC_; }

  // command
  void setLastCommandCode(uint16_t v) { lastCommandCode_ = v; }
  
private:
  std::vector<uint8_t> telemetry_;
  uint16_t telemetryType_;
  timeval timeNow_;
  uint32_t telemIndex_ = 0;
  // info
  uint32_t eventCount_;
  uint32_t triggerCount_;
  uint16_t chamberPressure_;
  std::vector<uint16_t> chamberTemperature_;
  uint16_t valveTemperature_;
  uint16_t outerTemperature_;
  int32_t TPCHVSetting_;
  uint16_t TPCHVMeasure_;
  int32_t PMTHVSetting_;
  uint16_t PMTHVMeasure_;
  int16_t CPUTemperature_;  
  std::vector<double> envTemperature_;
  std::vector<double> envHumidity_;
  std::vector<double> envPressure_;
  // acceleration
  int16_t mainCurrent_;
  int16_t mainVoltage_;
  uint32_t lastCommandIndex_;
  uint16_t lastCommandCode_;
  uint16_t commandRejectCount_;
  uint16_t softwareErrorCode_;
  
  uint32_t eventID_;
  std::vector<int16_t> eventHeader_;
  std::vector<std::vector<int16_t>> eventData_;
  std::vector<int16_t> RTDTemperatureADC_;
  
};

} /* namespace gramsballoon */

#endif /* TelemetryDefinition_H */
