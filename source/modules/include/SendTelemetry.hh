/**
 * Regulate high voltage via Analog Discovery.
 *
 * @author Tsubasa Tamba, Shota Arai
 * @date 2023-03-25
 */


#ifndef SendTelemetry_H
#define SendTelemetry_H 1

#include <anlnext/BasicModule.hh>
#include "TelemetryGenerator.hh"
#include "GetEnvironmentalData.hh"
#include "ReadWaveform.hh"
#include "MeasureTemperatureWithRTDSensor.hh"
#include "SerialCommunication.hh"

class ReadWaveform;
class GetEnvironmentalData;
class MeasureTemperatureWithRTDSensor;

class SendTelemetry : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(SendTelemetry, 1.0);

public:
  SendTelemetry();
  virtual ~SendTelemetry();

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_finalize() override;

  TelemetryGenerator* GetTelemetryGenerator() { return telemgen_.get(); }
  void setTelemetryType(int v) { telemetryType_ = v; }
  
  

private:
  std::unique_ptr<TelemetryGenerator> telemgen_ = nullptr;
  std::vector<std::string> getEnvironmentalDataModuleNames_;
  ReadWaveform* readWaveform_ = nullptr;
  std::string readWaveformModuleName_;
  std::vector<std::string> measureTemperatureModuleNames_;
  int telemetryType_ = 1;

  // communication
  std::unique_ptr<SerialCommunication> sc_;
  std::string serialPath_;
  speed_t baudrate_ = B9600;
  char openMode_ = O_RDWR;

};

#endif /* SendTelemetry_H */
