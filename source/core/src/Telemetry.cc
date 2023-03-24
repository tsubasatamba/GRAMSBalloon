#include "Telemetry.hh"
#include <thread>
#include <chrono>


Telemetry::Telemetry()
{
}

void Telemetry::generateTelemetry(int telem_type)
{
  clear();
  std::vector<uint8_t> start_code = {0xe, 0xb, 0x9, 0x0};
  addVector(start_code);
  addValue(telem_type);
  gettimeofday(&time_now, NULL);
  addValue(time_now.tv_sec);
  addValue(time_now.tv_usec);
  addValue(telemIndex_);
  telemIndex_++;
  
  if (telem_type==TELEM_TYPE_NORMAL) {
    generateTelemetryNormal();
  }
  else if (telem_type==TELEM_TYPE_WAVE) {
    generateTelemetryWave();
  }
  else {
    std::cerr << "telemetry type not set appropriately: telem_type = " << telem_type << std::endl;
    return;
  }
  writeMD5();
  std::vector<uint8_t> end_code = {0xc, 0x5, 0xc, 0x5};
  addVector(end_code);
}

void Telemetry::generateTelemetryNormal()
{
  addValue((uint16_t)0); // DAQ event count
  addValue((uint16_t)0); // Trigger event count
  addValue((uint16_t)0); // Chamber pressure
  addValue((uint16_t)0); // Chamber temperature

  writeRTDTemperature();
  addValue((uint16_t)0); // CPU temperature
  writeEnvironmentalData();
}

void Telemetry::generateTelemetryWave()
{
  addValue(eventID_);
  addVector(eventHeader_);
  const int n = eventData_.size();
  for (int i=0; i<n; i++) {
    addVector(eventData_[i]);
  }
}

void Telemetry::writeRTDTemperature()
{
  const int buf_size = 5;
  const double scale = 100.0;
  const int n = max31865ioVec_.size();
  std::vector<int16_t> temperature(buf_size, -300);
  for (int i=0; i<n; i++) {
    temperature[i] = static_cast<int16_t>(max31865ioVec_[i]->Temperature() * scale);
  }
  addVector(temperature);
}

void Telemetry::writeEnvironmentalData()
{
  const int buf_size = 5;
  const int n = bme680ioVec_.size();
  std::vector<uint16_t> temperature(buf_size);
  std::vector<uint16_t> humidity(buf_size);
  std::vector<uint16_t> pressure(buf_size);

  for (int i=0; i<n; i++) {
    temperature[i] = bme680ioVec_[i]->SensorData()->temperature;
    humidity[i] = bme680ioVec_[i]->SensorData()->humidity;
    pressure[i] = bme680ioVec_[i]->SensorData()->pressure;
  }
  addVector(temperature);
  addVector(humidity);
  addVector(pressure);
}

void Telemetry::writeMD5()
{
  const int n = telem_.size();
  std::vector<unsigned char> md(MD5_DIGEST_LENGTH);
  MD5(&telem_[0], n, &md[0]);
  addVector(md);
}

void Telemetry::clear()
{
  telem_.clear();
}

template<typename T>
void Telemetry::addValue(T input)
{
  const int size = sizeof(T);
  for (int i=0; i<size; i++) {
    telem_.push_back(static_cast<uint8_t>(input & 0xff));
    input >>= 8;
  }
}

template<typename T>
void Telemetry::addVector(std::vector<T>& input)
{
  const int n = input.size();
  for (int i=0; i<n; i++) {
    addValue(input[i]);
  }
}