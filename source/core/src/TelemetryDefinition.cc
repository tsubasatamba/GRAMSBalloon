#include "TelemetryDefinition.hh"
#include <thread>
#include <chrono>

namespace gramsballoon {

TelemetryDefinition::TelemetryDefinition()
{
}

void TelemetryDefinition::generateTelemetry(int telemetry_type)
{
  clear();
  uint16_t start_code = 0xeb90;
  addValue<uint16_t>(start_code);
  addValue<uint16_t>(static_cast<uint16_t>(telemetry_type));
  gettimeofday(&timeNow_, NULL);
  addValue<int32_t>(static_cast<int32_t>(timeNow_.tv_sec));
  addValue<int32_t>(static_cast<int32_t>(timeNow_.tv_usec));
  addValue<uint32_t>(telemIndex_);
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
  addValue<uint16_t>(end_code);
}

void TelemetryDefinition::generateTelemetryNormal()
{
  addValue<uint32_t>(eventCount_); // Event count
  addValue<uint32_t>((uint32_t)0); // Trigger count
  addValue<uint16_t>((uint16_t)0); // Chamber pressure
  
  writeRTDTemperature();
  addValue<int32_t>((int32_t)0); // TPC High Voltage Setting
  addValue<int16_t>((int16_t)0); // TPC High Voltage measurement
  addValue<int32_t>((int32_t)0); // PMT High Voltage Setting
  addValue<int16_t>((int16_t)0); // PMT High Voltage measurement
  addValue<int16_t>((int16_t)0); // CPU temperature
  writeEnvironmentalData();
  for (int i=0; i<9; i++) {
    addValue<int16_t>((int16_t)0); // acceleration
  }
  addValue<int16_t>((int16_t)0); //main current
  addValue<int16_t>((int16_t)0); //main voltage
  addValue<uint32_t>((uint32_t)0); // last command index
  addValue<uint16_t>(lastCommandCode_); // last command code
  addValue<uint16_t>((uint16_t)0); //command reject count
  addValue<uint16_t>((uint16_t)0); //software error code
}

void TelemetryDefinition::generateTelemetryWave()
{
  addValue<uint32_t>(eventID_);
  addVector<int16_t>(eventHeader_);
  const int n = eventData_.size();
  for (int i=0; i<n; i++) {
    addVector<int16_t>(eventData_[i]);
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
  addVector<uint16_t>(temperature);
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
  addVector<int16_t>(temperature);
  addVector<uint16_t>(humidity);
  addVector<uint16_t>(pressure);
}

void TelemetryDefinition::writeCRC16()
{
  uint16_t crc = calcCRC16(telemetry_);
  addValue<uint16_t>(crc);
}

void TelemetryDefinition::interpret()
{
  telemetryType_ = getValue<uint16_t>(2);
  timeNow_.tv_sec = getValue<int32_t>(4);
  timeNow_.tv_usec = getValue<int32_t>(8);
  telemIndex_ = getValue<uint32_t>(12);
  eventCount_ = getValue<uint32_t>(16);
  triggerCount_ = getValue<uint32_t>(20);
  chamberPressure_ = getValue<uint16_t>(24);
  chamberTemperature_.resize(3);
  getVector<uint16_t>(26, 3, chamberTemperature_);
  valveTemperature_ = getValue<uint16_t>(32);
  outerTemperature_ = getValue<uint16_t>(34);
  TPCHVSetting_ = getValue<int32_t>(36);
  TPCHVMeasure_ = getValue<uint16_t>(40);
  PMTHVSetting_ = getValue<int32_t>(42);
  PMTHVMeasure_ = getValue<uint16_t>(46);
  CPUTemperature_ = getValue<int16_t>(48);
  
  envTemperature_.resize(5);
  envHumidity_.resize(5);
  envPressure_.resize(5);
  std::vector<int16_t> temp(5);
  std::vector<uint16_t> hum(5);
  std::vector<uint16_t> pre(5);
  getVector<int16_t>(50, 5, temp);
  getVector<uint16_t>(60, 5, hum);
  getVector<uint16_t>(70, 5, pre);
  for (int i=0; i<5; i++) {
    envTemperature_[i] = static_cast<double>(temp[i])*0.1;
    envHumidity_[i] = static_cast<double>(hum[i])*0.1;
    envPressure_[i] = static_cast<double>(pre[i])*0.1;
  }
  // acceleration
  mainCurrent_ = getValue<int16_t>(98);
  mainVoltage_ = getValue<int16_t>(100);
  lastCommandIndex_ = getValue<uint32_t>(102);
  lastCommandCode_ = getValue<uint16_t>(106);
  commandRejectCount_ = getValue<uint16_t>(108);
  softwareErrorCode_ = getValue<uint16_t>(110);


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

template<typename T>
T TelemetryDefinition::getValue(int index)
{
  const int n = telemetry_.size();
  const int byte = sizeof(T);
  if (index+byte>n) {
    std::cerr << "TelemetryDefinition::getValue error: out of range" << std::endl;
    std::cerr << "telemetry_.size() = " << n << ", index = " << index << ", byte = " << byte << std::endl;
    return static_cast<T>(0);
  }
  if (byte > 4) {
    std::cerr << "TelemetryDefinition::getValue error: typename error" << std::endl;
    std::cerr << "byte should be equal to or less than 4: byte = " << byte << std::endl;
    return static_cast<T>(0);
  }

  uint32_t v = 0;
  for (int i=0; i<byte; i++) {
    const int shift = 8 * (byte-1-i);
    v |= telemetry_[i] << (shift);
  }
  return static_cast<T>(v);
}

template<typename T>
void TelemetryDefinition::getVector(int index, int num, std::vector<T>& vec)
{
  const int n = telemetry_.size();
  const int byte = sizeof(T);
  if (index+byte*num > n) {
    std::cerr << "TelemetryDefinition::getVector error: out of range" << std::endl;
    std::cerr << "telemetry_.size() = " << n << ", index = " << index << ", byte = " << byte
    << ", num = " << num << std::endl;
    return;
  }
  if (byte > 4) {
    std::cerr << "TelemetryDefinition::getVector error: typename error" << std::endl;
    std::cerr << "byte should be equal to or less than 4: byte = " << byte << std::endl;
    return;
  }
  vec.clear();
  for (int i=0; i<num; i++) {
    T v = getValue<T>(index);
    vec.push_back(v);
    index += byte;
  }
}

} /* namespace gramsballoon */
