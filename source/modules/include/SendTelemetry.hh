/**
 * Regulate high voltage via Analog Discovery.
 *
 * @author Tsubasa Tamba, Shota Arai
 * @date 2023-03-25
 */


#ifndef SendTelemetry_H
#define SendTelemetry_H 1

#include <anlnext/BasicModule.hh>
#include "TelemetryDefinition.hh"
#include "GetEnvironmentalData.hh"
#include "ReadWaveform.hh"
#include "MeasureTemperatureWithRTDSensor.hh"
#include "SerialCommunication.hh"

class ReadWaveform;
class GetEnvironmentalData;
class MeasureTemperatureWithRTDSensor;

namespace GRAMSBalloon {

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

  void inputInfo();
  void inputEnvironmentalData();
  void inputTemperatureData();

  void setTelemetryType(int v) { telemetryType_ = v; }

  void setEventID(int v) { telemdef_->setEventID(v); };
  void setEventHeader(const std::vector<short>& v) { telemdef_->setEventHeader(v); }
  void setEventData(const std::vector<std::vector<short>>& v) { telemdef_->setEventData(v); }
  int EventID() { return telemdef_->EventID(); }
  const std::vector<short>& EventHeader() const { return telemdef_->EventHeader(); }
  const std::vector<std::vector<short>>& EventData() const { return telemdef_->EventData(); }

private:
  std::unique_ptr<TelemetryDefinition> telemdef_ = nullptr;
  int telemetryType_ = 1;

  // access to other classes
  std::vector<std::string> getEnvironmentalDataModuleNames_;
  std::vector<GetEnvironmentalData*> getEnvironmentalDataVec_;
  std::string readWaveformModuleName_;
  ReadWaveform* readWaveform_ = nullptr;
  std::vector<std::string> measureTemperatureModuleNames_;
  std::vector<MeasureTemperatureWithRTDSensor*> measureTemperatureVec_;

  // communication
  std::unique_ptr<SerialCommunication> sc_;
  std::string serialPath_;
  speed_t baudrate_ = B9600;
  char openMode_ = O_RDWR;

};

} /* namespace GRAMSBalloon */

#endif /* SendTelemetry_H */
