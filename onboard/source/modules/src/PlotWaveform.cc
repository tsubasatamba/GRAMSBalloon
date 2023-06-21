#include "PlotWaveform.hh"
#include "DateManager.hh"
#include <TH1.h>
#include <TCanvas.h>

using namespace anlnext;

namespace gramsballoon {

PlotWaveform::PlotWaveform()
{
  imageName_ = "waveform.png";
}

PlotWaveform::~PlotWaveform() = default;

ANLStatus PlotWaveform::mod_define()
{
  define_parameter("image_name", &mod_class::imageName_);
  define_parameter("chatter", &mod_class::chatter_);
  
  return AS_OK;
}

ANLStatus PlotWaveform::mod_initialize()
{
  const std::string interpreter_module_name = "InterpretTelemetry";
  if (exist_module(interpreter_module_name)) {
    get_module_NC(interpreter_module_name, &interpreter_);
  }
  
  return AS_OK;
}

ANLStatus PlotWaveform::mod_analyze()
{
  
  return AS_OK;
}

ANLStatus PlotWaveform::mod_finalize()
{
  return AS_OK;
}

void PlotWaveform::makeImage()
{
  const int num_channels = 4;
  const std::vector<std::vector<int16_t> >& event_data = interpreter_->Telemdef()->EventData();
  if (num_channels!=static_cast<int>(event_data.size())) {
    std::cerr << "event_data size is not correct: it should be 4 but event_data_size = " << event_data.size() << std::endl;
    return;
  }
  
  const double sample_frequency = interpreter_->Telemdef()->SampleFrequency();
  const std::vector<double> adc_range = interpreter_->Telemdef()->ADCRange();
  const std::vector<double> adc_offset = interpreter_->Telemdef()->ADCOffset();

  const int nx = event_data[0].size();
  const double xmin = 0.0;
  const double xmax = (1.0/sample_frequency) * nx;

  std::vector<TH1D*> histograms(num_channels);
  std::vector<std::string> titles = {"PMT", "TPC1", "TPC2", "TPC3"};
  for (int i=0; i<num_channels; i++) {
    histograms[i] = new TH1D(titles[i].c_str(), titles[i].c_str(), event_data.size(), xmin, xmax);
  }
  for (int i=0; i<num_channels; i++) {
    const int num_bins = event_data[i].size();
    for (int j=0; j<num_bins; j++) {
      const double v = (event_data[i][j] * adc_range[i]) / 65536.0 + adc_offset[i];
      histograms[i]->Fill(j+1, v);
    }
  }

  TCanvas* canv = new TCanvas();
  canv -> Divide(2, 2);
  
  for (int i=0; i<num_channels; i++) {
    canv->cd(i+1);
    histograms[i]->Draw("AP");
  }
  canv->SaveAs(imageName_.c_str());

  
  for (int i=0; i<num_channels; i++) {
    delete(histograms[i]);
  }
  delete(canv);
  

}

} // namespace gramsballoon
