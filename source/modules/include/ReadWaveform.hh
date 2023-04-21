/**
 * Regulate high voltage via Analog Discovery.
 *
 * @author Tsubasa Tamba, Masato Kimura
 * @date 2023-03-22
 */


#ifndef ReadWaveform_H
#define ReadWaveform_H 1

#include<fstream>
#include <anlnext/BasicModule.hh>
#include "AnalogDiscoveryManager.hh"
#include "DAQIO.hh"
#include "SendTelemetry.hh"
#include "TelemetryDefinition.hh"

class SendTelemetry;

namespace gramsballoon {

class ReadWaveform : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(ReadWaveform, 1.0);
  ENABLE_PARALLEL_RUN();

public:
  ReadWaveform();
  virtual ~ReadWaveform();
protected:
  ReadWaveform(const ReadWaveform& r) = default;

public:  
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_finalize() override;

  void createNewOutputFile();
  void closeOutputFile();
  void writeData();

  const std::vector<std::vector<int16_t>>& EventData() const { return eventData_; }
  void setOndemand(bool v) { ondemand_ = v; }
  DAQIO* getDAQIO() { return daqio_.get(); }

  uint32_t EventCount() { return daqio_->EventCount(); }

private:
  std::string ADManagerName_ = "";
  AnalogDiscoveryManager* ADManager_ = nullptr;
  std::shared_ptr<DAQIO> daqio_ = nullptr;
  int trigDevice_ = 0;
  int trigChannel_ = 0;
  int trigMode_ = 2;
  double trigLevel_ = 0.0; // volt
  double trigPosition_ = 0.0; // us
  double timeWindow_ = 10.0; // us
  double sampleFreq_ = 2.0; //MHz
  std::vector<double> adcRangeList_; // volt
  std::vector<double> adcOffsetList_;

  std::vector<int16_t> eventHeader_;
  std::vector<std::vector<int16_t>> eventData_;
  std::string outputFilenameBase_ = "output";
  int numEventsPerFile_ = 100;
  std::shared_ptr<std::ofstream> ofs_;
  int fileID_ = 0;
  bool ondemand_ = false;
  SendTelemetry* sendTelemetry_;
};

} /* namespace gramsballoon */

#endif /* ReadWaveform_H */
