#include "TelemetryGenerator.hh"
#include <thread>
#include <chrono>


TelemetryGenerator::TelemetryGenerator()
{
}

void TelemetryGenerator::generateTelemetry(int telemetry_type)
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
  
  if (telemetry_type==TELEM_TYPE_NORMAL) {
    generateTelemetryNormal();
  }
  else if (telemetry_type==TELEM_TYPE_WAVE) {
    generateTelemetryWave();
  }
  else {
    std::cerr << "telemetry type not set appropriately: telemetry_type = " << telemetry_type << std::endl;
    return;
  }
  writeMD5();
  std::vector<uint8_t> end_code = {0xc5, 0xc5};
  addVector(end_code);
}

void TelemetryGenerator::generateTelemetryNormal()
{
  addValue((uint32_t)0); // DAQ event count
  addValue((uint32_t)0); // Trigger event count
  addValue((uint32_t)0); // Chamber pressure
  addValue((int32_t)0); // Chamber temperature

  writeRTDTemperature();
  addValue((int32_t)0); // CPU temperature
  writeEnvironmentalData();
}

void TelemetryGenerator::generateTelemetryWave()
{
  addValue(eventID_);
  addVector(eventHeader_);
  const int n = eventData_.size();
  for (int i=0; i<n; i++) {
    addVector(eventData_[i]);
  }
}

void TelemetryGenerator::writeRTDTemperature()
{
  const int buf_size = 5;
  const double scale = 100.0;
  const int n = max31865ioVec_.size();
  std::vector<int32_t> temperature(buf_size, -300);
  for (int i=0; i<n; i++) {
    if (i==buf_size) break;
    temperature[i] = static_cast<int32_t>(max31865ioVec_[i]->Temperature() * scale);
  }
  addVector(temperature);
}

void TelemetryGenerator::writeEnvironmentalData()
{
  const int buf_size = 5;
  const int n = bme680ioVec_.size();
  std::vector<int32_t> temperature(buf_size);
  std::vector<uint32_t> humidity(buf_size);
  std::vector<uint32_t> pressure(buf_size);

  for (int i=0; i<n; i++) {
    if (i==buf_size) break;
    temperature[i] = static_cast<int32_t>(bme680ioVec_[i]->SensorData()->temperature * 100.0);
    humidity[i] = static_cast<uint32_t>(bme680ioVec_[i]->SensorData()->humidity * 100.0);
    pressure[i] = static_cast<uint32_t>(bme680ioVec_[i]->SensorData()->pressure * 10.0);
  }
  addVector(temperature);
  addVector(humidity);
  addVector(pressure);
}

void TelemetryGenerator::writeMD5()
{
  const int n = telemetry_.size();
  std::vector<unsigned char> md(MD5_DIGEST_LENGTH);
  MD5(&telemetry_[0], n, &md[0]);
  addVector(md);
}

void TelemetryGenerator::clear()
{
  telemetry_.clear();
}

template<typename T>
void TelemetryGenerator::addValue(T input)
{
  const int size = sizeof(T);
  for (int i=0; i<size; i++) {
    telemetry_.push_back(static_cast<uint8_t>(input & 0xff));
    input >>= 8;
  }
}

template<typename T>
void TelemetryGenerator::addVector(std::vector<T>& input)
{
  const int n = input.size();
  for (int i=0; i<n; i++) {
    addValue(input[i]);
  }
}