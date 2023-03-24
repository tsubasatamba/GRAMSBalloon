#ifndef TelemetryGenerator_H
#define TelemetryGenerator_H 1


#include <iostream>
#include <vector>
#include <sys/time.h>
#include <openssl/md5.h>
#include "BME680IO.hh"
#include "MAX31865IO.hh"
#include "DAQIO.hh"

#define TELEM_TYPE_NORMAL 1
#define TELEM_TYPE_WAVE 2

/**
 * A class to define telemetry
 *
 * @author Tsubasa Tamba, Shota Arai
 * @date 2023-03-23
 */
class TelemetryGenerator
{
public:
  TelemetryGenerator();
  void generateTelemetry(int telem_type);
  void generateTelemetryNormal();
  void generateTelemetryWave();
  void writeRTDTemperature();
  void writeEnvironmentalData();
  void writeMD5();
  //void InterpretTelemetry();
  void clear();

  
  template<typename T> void addValue(T input);
  template<typename T> void addVector(std::vector<T>& input);

  void setEventID(int v) { eventID_ = v; };
  void setEventHeader(std::vector<short> v) { eventHeader_ = v; }
  void setEventData(std::vector<std::vector<short>> v) { eventData_ = v; }

  const std::vector<uint8_t>& Telemetry() const { return telemetry_; }

  //set access to other io
  void addBME680IO(BME680IO* io) { bme680ioVec_.push_back(io); }
  void setDAQIO(DAQIO* io) { daqio_ = io; }
  void addMAX31865IO(MAX31865IO* io) { max31865ioVec_.push_back(io); }

  
private:
  std::vector<uint8_t> telemetry_;
  timeval time_now;
  int telemIndex_ = 0;

  // access to other classes
  std::vector<BME680IO*> bme680ioVec_;
  DAQIO* daqio_ = nullptr;
  int eventID_;
  std::vector<short> eventHeader_;
  std::vector<std::vector<short>> eventData_;
  std::vector<MAX31865IO*> max31865ioVec_;
};



#endif /* TelemetryGenerator_H */
