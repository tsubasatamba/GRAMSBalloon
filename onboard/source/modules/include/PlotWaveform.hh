#ifndef PlotWaveform_H
#define PlotWaveform_H 1

#include <anlnext/BasicModule.hh>
#include "TelemetryDefinition.hh"
#include "InterpretTelemetry.hh"

namespace gramsballoon {

class InterpretTelemetry;

class PlotWaveform : public anlnext::BasicModule {
  DEFINE_ANL_MODULE(PlotWaveform, 1.0);

public:
  PlotWaveform();
  virtual ~PlotWaveform();

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_finalize() override;

  void makeImage(std::vector<std::string>& image_filenames);

private:
  InterpretTelemetry* interpreter_ = nullptr;
  std::string wfImageName_;
  std::string wfAutorangeImageName_;
  std::string pmtImageName_;
  int chatter_ = 0;
};

} // namespace gramsballoon
#endif // PlotWaveform_H
