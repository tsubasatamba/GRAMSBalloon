#include "TelemetryDefinition.hh"
#include <thread>
#include <chrono>

namespace GRAMSBalloon {

TelemetryDefinition::TelemetryDefinition()
{
}

void TelemetryDefinition::generateTelemetry(int telemetry_type)
{
  clear();
  uint16_t start_code = 0xeb90;
  addValue(start_code);
  addValue(static_cast<uint16_t>(telemetry_type));
  gettimeofday(&time_now, NULL);
  addValue(static_cast<int32_t>(time_now.tv_sec));
  addValue(static_cast<int32_t>(time_now.tv_usec));
  addValue(telemIndex_);
  telemIndex_++;
  
  if (telemetry_type==static_cast<int>(TelemetryType::normal)) {
    generateTelemetryNormal();
  }
  else if (telemetry_type==static_cast<int>(TelemetryType::wave)) {
    generateTelemetryWave();
  }
  else if (telemetry_type==static_cast<int>(TelemetryType::status)) {
    generateTelemetryStatus();
  }
  else {
    std::cerr << "telemetry type not set appropriately: telemetry_type = " << telemetry_type << std::endl;
    return;
  }
  writeCRC16();
  uint16_t end_code = 0xc5c5;
  addValue(end_code);
}

void TelemetryDefinition::generateTelemetryNormal()
{
  addValue((uint32_t)0); // Event count
  addValue((uint32_t)0); // Trigger count
  addValue((uint16_t)0); // Chamber pressure
  
  writeRTDTemperature();
  addValue((int32_t)0); // TPC High Voltage Setting
  addValue((int16_t)0); // TPC High Voltage measurement
  addValue((int32_t)0); // PMT High Voltage Setting
  addValue((int16_t)0); // PMT High Voltage measurement
  addValue((int16_t)0); // CPU temperature
  writeEnvironmentalData();
  for (int i=0; i<9; i++) {
    addValue((int16_t)0); // acceleration
  }
  addValue((int16_t)0); //main current
  addValue((int16_t)0); //main voltage
  addValue((uint32_t)0); // last command index
  addValue(lastCommandCode_); // last command code
  addValue((uint16_t)0); //command reject count
  addValue((uint16_t)0); //software error code
}

void TelemetryDefinition::generateTelemetryWave()
{
  addValue(eventID_);
  addVector(eventHeader_);
  const int n = eventData_.size();
  for (int i=0; i<n; i++) {
    addVector(eventData_[i]);
  }
}

void TelemetryDefinition::generateTelemetryStatus()
{

}

void TelemetryDefinition::writeRTDTemperature()
{
  const int buf_size = 5;
  const int n = RTDTemperatureADC_.size();
  std::vector<uint16_t> temperature(buf_size, 0);
  for (int i=0; i<n; i++) {
    if (i==buf_size) break;
    temperature[i] = RTDTemperatureADC_[i];
  }
  addVector(temperature);
}

void TelemetryDefinition::writeEnvironmentalData()
{
  const int buf_size = 5;
  const int n = envTemperature_.size();
  std::vector<int16_t> temperature(buf_size);
  std::vector<uint16_t> humidity(buf_size);
  std::vector<uint16_t> pressure(buf_size);

  for (int i=0; i<n; i++) {
    if (i==buf_size) break;
    temperature[i] = static_cast<int16_t>(envTemperature_[i] * 100.0);
    humidity[i] = static_cast<uint16_t>(envHumidity_[i] * 100.0);
    pressure[i] = static_cast<uint16_t>(envPressure_[i] * 10.0);
  }
  addVector(temperature);
  addVector(humidity);
  addVector(pressure);
}

void TelemetryDefinition::writeCRC16()
{
  uint16_t crc = calcCRC16(telemetry_);
  addValue(crc);
}

void TelemetryDefinition::clear()
{
  telemetry_.clear();
}

template<typename T>
void TelemetryDefinition::addValue(T input)
{
  const int size = sizeof(T);
  uint32_t v = static_cast<uint32_t>(input);
  for (int i=0; i<size; i++) {
    const int shift = 8 * (size-1-i);
    telemetry_.push_back(static_cast<uint8_t>((v>>shift) & 0xff));
  }
}

template<typename T>
void TelemetryDefinition::addVector(std::vector<T>& input)
{
  const int n = input.size();
  for (int i=0; i<n; i++) {
    addValue(input[i]);
  }
}

} /* namespace GRAMSBalloon */