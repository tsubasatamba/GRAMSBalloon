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
  HK = 1,
  WF = 2,
  Status = 3
};

class TelemetryDefinition
{
public:
  TelemetryDefinition();
  void generateTelemetry();
  void generateTelemetryHK();
  void generateTelemetryWF();
  void generateTelemetryStatus();
  void writeRTDTemperature();
  void writeEnvironmentalData();
  void writeAccelerationData();
  void writeCRC16();
  void clear();
  void writeFile(const std::string& filename, bool append);

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

  // rtd
  void setRTDTemperatureADC(int index, int16_t v) { RTDTemperatureADC_[index] = v; }
  void resizeRTDTemperatureADC(int n) { RTDTemperatureADC_.resize(n); }
  std::vector<int16_t>& RTDTemperatureADC() { return RTDTemperatureADC_; }


  // setter
  void setTelemetryType(uint16_t v) { telemetryType_ = v; }
  void setEventCount(uint32_t v) { eventCount_ = v; }
  void setTriggerCount(uint32_t v) { triggerCount_ = v; }
  void setChamberPressure(uint16_t v) { chamberPressure_ = v; }
  void setChamberTemperature(const std::vector<uint16_t>& v) { chamberTemperature_ = v; } //should be fixed
  void setChamberTemperature(int index, uint16_t v) { if (index<static_cast<int>(chamberTemperature_.size())) chamberTemperature_[index] = v; }
  void resizeChamberTemperature(int n) { chamberTemperature_.resize(n); }
  void setValveTemperature(uint16_t v) { valveTemperature_ = v; }
  void setOuterTemperature(uint16_t v) { outerTemperature_ = v; }
  void setTPCHVSetting(double v) { TPCHVSetting_ = v; }
  void setTPCHVMeasure(uint16_t v) {TPCHVMeasure_ = v; }
  void setPMTHVSetting(double v) { PMTHVSetting_ = v; }
  void setPMTHVMeasure(uint16_t v) {PMTHVMeasure_ = v; }
  void setCPUTemperature(double v) { CPUTemperature_ = v; }
  void setEnvTemperature(const std::vector<double>& v) { envTemperature_ = v; }
  void setEnvTemperature(int index, double v) { if (index<static_cast<int>(envTemperature_.size())) envTemperature_[index] = v; }
  void resizeEnvTemperature(int n) { envTemperature_.resize(n); }
  void setEnvHumidity(const std::vector<double>& v) { envHumidity_ = v; }
  void setEnvHumidity(int index, double v) { if (index<static_cast<int>(envHumidity_.size())) envHumidity_[index] = v; }
  void resizeEnvHumidity(int n) { envHumidity_.resize(n); }
  void setEnvPressure(const std::vector<double>& v) { envPressure_ = v; }
  void setEnvPressure(int index, double v) { if (index<static_cast<int>(envPressure_.size())) envPressure_[index] = v; }
  void resizeEnvPressure(int n) { envPressure_.resize(n); }
  void setAcceleration(const std::vector<float>& v) { acceleration_ = v; }
  void setAcceleration(int index, float v) { if (index<static_cast<int>(acceleration_.size())) acceleration_[index] = v; }
  void resizeAcceleration(int n) { acceleration_.resize(n); }
  void setGyro(const std::vector<float>& v) { gyro_ = v; }
  void setGyro(int index, float v) { if (index<static_cast<int>(gyro_.size())) gyro_[index] = v; }
  void resizeGyro(int n) { gyro_.resize(n); }
  void setMagnet(const std::vector<float>& v) { magnet_ = v; }
  void setMagnet(int index, float v) { if (index<static_cast<int>(magnet_.size())) magnet_[index] = v; }
  void resizeMagnet(int n) { magnet_.resize(n); }
  void setMainCurrent(uint16_t v) { mainCurrent_ = v; }
  void setMainVoltage(uint16_t v) { mainVoltage_ = v; }
  void setLastCommandIndex(uint32_t v) { lastCommandIndex_ = v; }
  void setLastCommandCode(uint16_t v) { lastCommandCode_ = v; }
  void setCommandRejectCount(uint16_t v) { commandRejectCount_ = v; }
  void setSoftwareErrorCode(uint64_t v) { softwareErrorCode_ = v; }
  void setEventID(uint32_t v) { eventID_ = v; }
  void setEventHeader(const std::vector<int16_t>& v) { eventHeader_ = v; }
  void setEventData(const std::vector<std::vector<int16_t>>& v) { eventData_ = v; }
  void setTriggerMode(uint16_t v) { triggerMode_ = v; }
  void setTriggerDevice(uint16_t v) { triggerDevice_ = v; }
  void setTriggerChannel(uint16_t v) { triggerChannel_ = v; }
  void setTriggerLevel(double v) { triggerLevel_ = v; }
  void setTriggerPosition(double v) { triggerPosition_ = v; }
  void setChannelMask(uint16_t v) { channelMask_ = v; }
  void setADCOffset(const std::vector<double>& v) { ADCOffset_ = v; }
  void setADCRange(const std::vector<double>& v) { ADCRange_ = v; }
  void setSDCapacity(uint64_t v) { SDCapacity_ = v; }
  
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
  double TPCHVSetting() { return TPCHVSetting_; }
  uint16_t TPCHVMeasure() { return TPCHVMeasure_; }
  double PMTHVSetting() { return PMTHVSetting_; }
  uint16_t PMTHVMeasure() { return PMTHVMeasure_; }
  double CPUTemperature() { return CPUTemperature_; }
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
  uint16_t MainCurrent() { return mainCurrent_; }
  uint16_t MainVoltage() { return mainVoltage_; }
  uint32_t LastCommandIndex() { return lastCommandIndex_; }
  uint16_t LastCommandCode() { return lastCommandCode_; }
  uint16_t CommandRejectCount() { return commandRejectCount_; }
  uint64_t SoftwareErrorCode() { return softwareErrorCode_; }
  uint32_t EventID() { return eventID_; }
  timeval EventTime() { return eventTime_; }
  const std::vector<int16_t>& EventHeader() const { return eventHeader_; }
  const std::vector<std::vector<int16_t>>& EventData() const { return eventData_; }
  uint16_t TriggerMode() { return triggerMode_; }
  uint16_t TriggerDevice() { return triggerDevice_; }
  uint16_t TriggerChannel() { return triggerChannel_; }
  double TriggerLevel() { return triggerLevel_; }
  double TriggerPosition() { return triggerPosition_; }
  uint16_t ChannelMask() { return channelMask_; }
  const std::vector<double>& ADCOffset() const {return ADCOffset_; }
  const std::vector<double>& ADCRange() const { return ADCRange_; }
  uint64_t SDCapacity() { return SDCapacity_; }

private:
  std::vector<uint8_t> telemetry_;
  // header
  uint16_t startCode_ = 0xeb90;
  uint16_t telemetryType_ = 0;
  timeval timeNow_;
  uint32_t telemetryIndex_ = 0;
  // footer
  uint16_t crc_ = 0;
  uint16_t stopCode_ = 0xc5c5;

  // HK
  uint32_t eventCount_ = 0;
  uint32_t triggerCount_ = 0;
  uint16_t chamberPressure_ = 0;
  std::vector<uint16_t> chamberTemperature_;
  uint16_t valveTemperature_ = 0;
  uint16_t outerTemperature_ = 0;
  double TPCHVSetting_ = 0.0;
  uint16_t TPCHVMeasure_ = 0;
  double PMTHVSetting_ = 0.0;
  uint16_t PMTHVMeasure_ = 0;
  double CPUTemperature_ = 0.0;
  std::vector<double> envTemperature_;
  std::vector<double> envHumidity_;
  std::vector<double> envPressure_;
  std::vector<float> acceleration_;
  std::vector<float> gyro_;
  std::vector<float> magnet_;
  uint16_t mainCurrent_ = 0;
  uint16_t mainVoltage_ = 0;
  uint32_t lastCommandIndex_ = 0;
  uint16_t lastCommandCode_ = 0;
  uint16_t commandRejectCount_ = 0;
  uint64_t softwareErrorCode_ = 0;
  
  // WF
  uint32_t eventID_ = 0;
  timeval eventTime_;
  std::vector<int16_t> eventHeader_;
  std::vector<std::vector<int16_t>> eventData_;

  // Status
  uint16_t triggerMode_ = 0;
  uint16_t triggerDevice_ = 0;
  uint16_t triggerChannel_ = 0;
  double triggerLevel_ = 0.0; // volt
  double triggerPosition_ = 0.0; // us
  uint16_t channelMask_ = 0;
  std::vector<double> ADCOffset_; // volt
  std::vector<double> ADCRange_; // volt
  uint64_t SDCapacity_ = 0;


  std::vector<int16_t> RTDTemperatureADC_;
  
};

} /* namespace gramsballoon */

#endif /* TelemetryDefinition_H */
