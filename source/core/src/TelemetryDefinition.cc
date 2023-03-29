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
  std::vector<uint8_t> start_code = {0xeb, 0x90};
  addVector(start_code);
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
  else {
    std::cerr << "telemetry type not set appropriately: telemetry_type = " << telemetry_type << std::endl;
    return;
  }
  writeMD5();
  std::vector<uint8_t> end_code = {0xc5, 0xc5};
  addVector(end_code);
  std::cout << "time_sec: " << time_now.tv_sec << std::endl;
  std::cout << "time_usec: " << time_now.tv_usec << std::endl;
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

void TelemetryDefinition::writeMD5()
{
  const int n = telemetry_.size();
  std::vector<unsigned char> md(MD5_DIGEST_LENGTH);
  MD5(&telemetry_[0], n, &md[0]);
  addVector(md);
}

void TelemetryDefinition::clear()
{
  telemetry_.clear();
}

template<typename T>
void TelemetryDefinition::addValue(T input)
{
  const int size = sizeof(T);
  for (int i=0; i<size; i++) {
    telemetry_.push_back(static_cast<uint8_t>(input & 0xff));
    input >>= 8;
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