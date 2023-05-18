#include "TelemetryDefinition.hh"
#include <thread>
#include <chrono>
#include <fstream>
#include "BinaryFileManipulater.hh"

namespace gramsballoon {

TelemetryDefinition::TelemetryDefinition()
{
  chamberTemperature_.resize(3);
  envTemperature_.resize(5);
  envHumidity_.resize(5);
  envPressure_.resize(5);
  acceleration_.resize(3);
  gyro_.resize(3);
  magnet_.resize(3);
  ADCOffset_.resize(4);
  ADCRange_.resize(4);
}

void TelemetryDefinition::generateTelemetry()
{
  clear();
  startCode_ = 0xeb90;
  addValue<uint16_t>(startCode_);
  addValue<uint16_t>(static_cast<uint16_t>(telemetryType_));
  gettimeofday(&timeNow_, NULL);
  addValue<int32_t>(static_cast<int32_t>(timeNow_.tv_sec));
  addValue<int32_t>(static_cast<int32_t>(timeNow_.tv_usec));
  addValue<uint32_t>(telemetryIndex_);
  telemetryIndex_++;
  
  if (telemetryType_==static_cast<int>(TelemetryType::HK)) {
    generateTelemetryHK();
  }
  else if (telemetryType_==static_cast<int>(TelemetryType::WF)) {
    generateTelemetryWF();
  }
  else if (telemetryType_==static_cast<int>(TelemetryType::Status)) {
    generateTelemetryStatus();
  }
  else {
    std::cerr << "telemetry type not set appropriately: telemetry_type = " << telemetryType_ << std::endl;
    return;
  }
  writeCRC16();
  stopCode_ = 0xc5c5;
  addValue<uint16_t>(stopCode_);
}

void TelemetryDefinition::generateTelemetryHK()
{
  addValue<uint32_t>(eventCount_);
  addValue<uint32_t>(triggerCount_);
  addValue<uint16_t>(chamberPressure_);
  
  writeRTDTemperature();  
  addValue<int32_t>(static_cast<int32_t>(TPCHVSetting_ / 1E-3));
  addValue<int16_t>(TPCHVMeasure_);
  addValue<int32_t>(static_cast<int32_t>(PMTHVSetting_ / 1E-3));
  addValue<int16_t>(PMTHVMeasure_);
  addValue<int16_t>(static_cast<int16_t>(CPUTemperature_ / 0.1));
  writeEnvironmentalData();
  writeAccelerationData();
  addValue<int16_t>(mainCurrent_); //main current
  addValue<int16_t>(mainVoltage_); //main voltage
  addValue<uint32_t>(lastCommandIndex_); // last command index
  addValue<uint16_t>(lastCommandCode_); // last command code
  addValue<uint16_t>(commandRejectCount_); //command reject count
  addValue<uint64_t>(softwareErrorCode_); //software error code
}

void TelemetryDefinition::generateTelemetryWF()
{
  addValue<uint32_t>(eventID_);
  for (int i=0; i<5; i++) {
    if (i==0) continue;
    addValue<int16_t>(eventHeader_[i]);
  }
  const int n = eventData_.size();
  for (int i=0; i<n; i++) {
    addVector<int16_t>(eventData_[i]);
  }
}

void TelemetryDefinition::generateTelemetryStatus()
{
  addValue<uint16_t>(triggerMode_);
  addValue<uint16_t>(triggerDevice_);
  addValue<uint16_t>(triggerChannel_);
  addValue<int32_t>(static_cast<int32_t>(triggerLevel_/1E-3));
  addValue<int32_t>(static_cast<int32_t>(triggerPosition_/1E-3));
  addValue<uint16_t>(channelMask_);
  const int n_offset = ADCOffset_.size();
  for (int i=0; i<n_offset; i++) {
    addValue<int32_t>(static_cast<int32_t>(ADCOffset_[i]/1E-3));
  }
  const int n_range = ADCRange_.size();
  for (int i=0; i<n_range; i++) {
    addValue<int32_t>(static_cast<int32_t>(ADCRange_[i]/1E-3));
  }
  addValue<uint64_t>(SDCapacity_);
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
  for (int i=0; i<3; i++) {
    setChamberTemperature(i, temperature[i]);
  }
  setValveTemperature(temperature[3]);
  setOuterTemperature(temperature[4]);
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
    temperature[i] = static_cast<int16_t>(envTemperature_[i] / 0.1);
    humidity[i] = static_cast<uint16_t>(envHumidity_[i] / 0.1);
    pressure[i] = static_cast<uint16_t>(envPressure_[i] / 10.0);
  }
  addVector<int16_t>(temperature);
  addVector<uint16_t>(humidity);
  addVector<uint16_t>(pressure);
}

void TelemetryDefinition::writeAccelerationData()
{
  const int buf_size = 3;
  std::vector<int16_t> accel(buf_size);
  std::vector<int16_t> gyro(buf_size);
  std::vector<int16_t> magnet(buf_size);
  for (int i=0; i<buf_size; i++) {
    if (i<static_cast<int>(acceleration_.size())) accel[i] = static_cast<int16_t>(acceleration_[i]/0.01);
    if (i<static_cast<int>(gyro_.size())) gyro[i] = static_cast<int16_t>(gyro_[i]/0.01);
    if (i<static_cast<int>(magnet_.size())) magnet[i] = static_cast<int16_t>(magnet_[i]/0.01);
  }
  addVector<int16_t>(accel);
  addVector<int16_t>(gyro);
  addVector<int16_t>(magnet);
}

void TelemetryDefinition::writeCRC16()
{
  uint16_t crc = calcCRC16(telemetry_);
  addValue<uint16_t>(crc);
}

bool TelemetryDefinition::setTelemetry(const std::vector<uint8_t>& v)
{
  const int n = v.size();
  if (n<6) {
    std::cerr << "Telemetry is too short!!: length = " << n << std::endl; 
    return false;
  }

  if (v[0]!=0xeb || v[1]!=0x90) {
    std::cerr << "start code incorect" << std::endl;
    return false;
  }
  if (v[n-2]!=0xc5 || v[n-1]!=0xc5) {
    std::cerr << "stop code incorrect" << std::endl;
    return false;
  }

  telemetry_ = v;
  std::vector<uint8_t> telem_without_fotter;
  for (int i=0; i<n-4; i++) {
    telem_without_fotter.push_back(v[i]);
  }
  
  uint16_t crc_calc = calcCRC16(telem_without_fotter);
  uint16_t crc_attached = getValue<uint16_t>(n-4);
  if (crc_calc != crc_attached) {
    std::cerr << "Invalid command: CRC16 not appropriate" << std::endl;
    return false;
  }

  uint16_t type = getValue<uint16_t>(2);
  if (type==1 && n!=122) {
    std::cerr << "Telemetry HK: Telemetry length is not correct: n = " << n << std::endl;
    return false;
  }

  return true;
}

void TelemetryDefinition::interpret()
{
  startCode_ = getValue<uint16_t>(0);
  telemetryType_ = getValue<uint16_t>(2);
  std::cout << "telemetry type: " << telemetryType_ << std::endl;
  timeNow_.tv_sec = getValue<int32_t>(4);
  timeNow_.tv_usec = getValue<int32_t>(8);
  telemetryIndex_ = getValue<uint32_t>(12);
  if (static_cast<int>(telemetryType_)==static_cast<int>(TelemetryType::HK)) {
    interpretHK();
  }
  else if (static_cast<int>(telemetryType_)==static_cast<int>(TelemetryType::WF)) {
    interpretWF();
  }
  else if (static_cast<int>(telemetryType_)==static_cast<int>(TelemetryType::Status)) {
    interpretStatus();
  }
  else {
    std::cerr << "could not interpret telemetry: telemetry_type = " << telemetryType_ << std::endl;
  }
}

void TelemetryDefinition::interpretHK()
{
  eventCount_ = getValue<uint32_t>(16);
  triggerCount_ = getValue<uint32_t>(20);
  chamberPressure_ = getValue<uint16_t>(24);
  chamberTemperature_.resize(3);
  getVector<uint16_t>(26, 3, chamberTemperature_);
  valveTemperature_ = getValue<uint16_t>(32);
  outerTemperature_ = getValue<uint16_t>(34);
  TPCHVSetting_ = static_cast<double>(getValue<int32_t>(36))*1E-3;
  TPCHVMeasure_ = getValue<uint16_t>(40);
  PMTHVSetting_ = static_cast<double>(getValue<int32_t>(42))*1E-3;
  PMTHVMeasure_ = getValue<uint16_t>(46);
  CPUTemperature_ = static_cast<double>(getValue<int16_t>(48)) * 0.1;
  
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
    envPressure_[i] = static_cast<double>(pre[i])*10.0;
  }

  acceleration_.resize(3);
  gyro_.resize(3);
  magnet_.resize(3);
  std::vector<int16_t> acc(3);
  std::vector<int16_t> gyr(3);
  std::vector<int16_t> mag(3);
  getVector<int16_t>(80, 3, acc);
  getVector<int16_t>(86, 3, gyr);
  getVector<int16_t>(92, 3, mag);
  for (int i=0; i<3; i++) {
    acceleration_[i] = static_cast<float>(acc[i]) * 0.01;
    gyro_[i] = static_cast<float>(gyr[i]) * 0.01;
    magnet_[i] = static_cast<float>(mag[i]) * 0.01;
  }

  mainCurrent_ = getValue<int16_t>(98);
  mainVoltage_ = getValue<int16_t>(100);
  lastCommandIndex_ = getValue<uint32_t>(102);
  lastCommandCode_ = getValue<uint16_t>(106);
  commandRejectCount_ = getValue<uint16_t>(108);
  softwareErrorCode_ = getValue<uint64_t>(110);
  crc_ = getValue<uint16_t>(118);
  stopCode_ = getValue<uint16_t>(120);
}

void TelemetryDefinition::interpretWF()
{
  const int total_size = telemetry_.size();
  const int header_size = 16;
  const int footer_size = 4;
  const int event_header_size = 12;
  const int event_size = (total_size-header_size-footer_size-event_header_size)/4;

  int index = header_size;
  eventHeader_.resize(event_header_size/sizeof(int16_t));
  getVector<int16_t>(index, event_header_size, eventHeader_);
  eventID_ = getValue<uint32_t>(index);
  eventTime_.tv_sec = getValue<int32_t>(index+4);
  eventTime_.tv_usec = getValue<int32_t>(index+8);
  index += event_header_size;
  
  eventData_.resize(4);
  for (int i=0; i<4; i++) {
    eventData_[i].resize(event_size/sizeof(int16_t));
    getVector<int16_t>(index, event_size, eventData_[i]);
    index += event_size;
  }

  crc_ = getValue<uint16_t>(index);
  stopCode_ = getValue<uint16_t>(index+2);
}

void TelemetryDefinition::interpretStatus()
{
  triggerMode_ = getValue<uint16_t>(16);
  triggerDevice_ = getValue<uint16_t>(18);
  triggerChannel_ = getValue<uint16_t>(20);
  triggerLevel_ = getValue<int32_t>(22) * 1E-3;
  triggerPosition_ = getValue<int32_t>(26) * 1E-3;
  channelMask_ = getValue<uint16_t>(30);

  std::vector<int32_t> offset(4);
  std::vector<int32_t> range(4);
  getVector<int32_t>(32, 4, offset);
  getVector<int32_t>(48, 4, range);
  ADCOffset_.resize(4);
  ADCRange_.resize(4);
  for (int i=0; i<4; i++) {
    ADCOffset_[i] = static_cast<double>(offset[i]) * 1E-3;
    ADCRange_[i] = static_cast<double>(range[i]) * 1E-3;
  }
  
  SDCapacity_ = getValue<uint64_t>(64);

  crc_ = getValue<uint16_t>(72);
  stopCode_ = getValue<uint16_t>(74);
}

void TelemetryDefinition::clear()
{
  telemetry_.clear();
}

void TelemetryDefinition::writeFile(const std::string& filename, bool append)
{
  writeVectorToBinaryFile<uint8_t>(filename, append, telemetry_);
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
  if (byte > 8) {
    std::cerr << "TelemetryDefinition::getValue error: typename error" << std::endl;
    std::cerr << "byte should be equal to or less than 8: byte = " << byte << std::endl;
    return static_cast<T>(0);
  }

  uint32_t v = 0;
  for (int i=0; i<byte; i++) {
    const int j = index + i;
    const int shift = 8 * (byte-1-i);
    v |= telemetry_[j] << (shift);
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
  if (byte > 8) {
    std::cerr << "TelemetryDefinition::getVector error: typename error" << std::endl;
    std::cerr << "byte should be equal to or less than 8: byte = " << byte << std::endl;
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
