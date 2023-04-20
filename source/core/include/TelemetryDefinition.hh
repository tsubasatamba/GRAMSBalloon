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

  bool setTelemetry(const std::vector<uint8_t>& v);
  void interpret();
  void interpretHK();
  void interpretWF();
  void interpretStatus();
  
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

  
  // getter
  uint16_t StartCode() { return startCode_; }
  uint16_t TelemetryType() { return telemetryType_; }
  timeval TimeNow() { return timeNow_; }
  uint32_t TelemetryIndex() { return telemetryIndex_; }
  uint16_t CRC() { return crc_; }
  uint16_t StopCode() { return stopCode_; }
  uint32_t EventCount() { return eventCount_; }
  uint32_t TriggerCount() { return triggerCount_; }
  uint16_t ChamberPressure() { return chamberPressure_; }
  const std::vector<uint16_t>& ChamberTemperature() const { return chamberTemperature_; }
  std::vector<uint16_t>& ChamberTemperature() { return chamberTemperature_; }
  uint16_t ChamberTemperature(int index) { return (index<(int)chamberTemperature_.size()) ? chamberTemperature_[index] : 0 ; }
  uint16_t ValveTemperature() { return valveTemperature_; }
  uint16_t OuterTemperature() { return outerTemperature_; }
  int32_t TPCHVSetting() { return TPCHVSetting_; }
  uint16_t TPCHVMeasure() { return TPCHVMeasure_; }
  int32_t PMTHVSetting() { return PMTHVSetting_; }
  uint16_t PMTHVMeasure() { return PMTHVMeasure_; }
  int16_t CPUTemperature() { return CPUTemperature_; }
  const std::vector<double>& EnvTemperature() const { return envTemperature_; }
  std::vector<double>& EnvTemperature() { return envTemperature_; }
  double EnvTemperature(int index) { return index<(int)envTemperature_.size() ? envTemperature_[index] : 0 ; }
  const std::vector<double>& EnvHumidity() const { return envHumidity_; }
  std::vector<double>& EnvHumidity() { return envHumidity_; }
  double EnvHumidity(int index) { return index<(int)envHumidity_.size() ? envHumidity_[index] : 0 ; }
  const std::vector<double>& EnvPressure() const { return envPressure_; }
  std::vector<double>& EnvPressure() { return envPressure_; }
  double EnvPressure(int index) { return index<(int)envPressure_.size() ? envPressure_[index] : 0 ; }
  const std::vector<float>& Acceleration() const { return acceleration_; }
  std::vector<float>& Acceleration() { return acceleration_; }
  float Acceleration(int index) { return index<(int)acceleration_.size() ? acceleration_[index] : 0 ; }
  const std::vector<float>& Gyro() const { return gyro_; }
  std::vector<float>& Gyro() { return gyro_; }
  float Gyro(int index) { return index<(int)gyro_.size() ? gyro_[index] : 0 ; }
  const std::vector<float>& Magnet() const { return magnet_; }
  std::vector<float>& Magnet() { return magnet_; }
  float Magnet(int index) { return index<(int)magnet_.size() ? magnet_[index] : 0 ; }
  int16_t MainCurrent() { return mainCurrent_; }
  int16_t MainVoltage() { return mainVoltage_; }
  uint32_t LastCommandIndex() { return lastCommandIndex_; }
  uint16_t LastCommandCode() { return lastCommandCode_; }
  uint16_t CommandRejectCount() { return commandRejectCount_; }
  uint16_t SoftwareErrorCode() { return softwareErrorCode_; }

private:
  std::vector<uint8_t> telemetry_;
  // header
  uint16_t startCode_;
  uint16_t telemetryType_;
  timeval timeNow_;
  uint32_t telemetryIndex_;
  // footer
  uint16_t crc_;
  uint16_t stopCode_;

  // HK
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
  std::vector<float> acceleration_;
  std::vector<float> gyro_;
  std::vector<float> magnet_;
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
