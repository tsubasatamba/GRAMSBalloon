#ifndef Telemetry_H
#define Telemetry_H 1


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
class Telemetry
{
public:
  Telemetry();
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

  
private:
  std::vector<uint8_t> telem_;
  timeval time_now;
  int telemIndex_ = 0;

  // access to other classes
  std::vector<BME680IO*> bme680ioVec_;
  std::vector<MAX31865IO*> max31865ioVec_;
  DAQIO* daqio_;
  int eventID_;
  std::vector<short> eventHeader_;
  std::vector<std::vector<short>> eventData_;
};



#endif /* Telemetry_H */
