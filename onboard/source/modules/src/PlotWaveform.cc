#include "PlotWaveform.hh"
#include "DateManager.hh"
#include <TH1.h>
#include <TCanvas.h>
#include <TStyle.h>

using namespace anlnext;

namespace gramsballoon {

PlotWaveform::PlotWaveform()
{
  wfImageName_ = "waveform_all.png";
  wfAutorangeImageName_ = "waveform_all_autorange.png";
  pmtImageName_ = "waveform_pmt.png";
}

PlotWaveform::~PlotWaveform() = default;

ANLStatus PlotWaveform::mod_define()
{
  define_parameter("wf_image_name", &mod_class::wfImageName_);
  define_parameter("wf_autorange_image_name", &mod_class::wfAutorangeImageName_);
  define_parameter("pmt_image_name", &mod_class::pmtImageName_);
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

void PlotWaveform::makeImage(std::vector<std::string>& image_filenames)
{
  const int num_channels = 4;
  const std::vector<std::vector<int16_t> >& event_data = interpreter_->Telemdef()->EventData();
  if (num_channels!=static_cast<int>(event_data.size())) {
    std::cerr << "event_data size is not correct: it should be 4 but event_data_size = " << event_data.size() << std::endl;
    return;
  }
  
  const double sample_frequency = interpreter_->Telemdef()->SampleFrequency();
  std::vector<double> adc_range(num_channels, 5.5);
  for (int i=0; i<num_channels; i++) {
    const double range = interpreter_->Telemdef()->ADCRange()[i];
    const double eps = 1E-9;
    if (std::abs(range)>eps) {
      adc_range[i] = range;
    }
  }
  const std::vector<double>& adc_offset = interpreter_->Telemdef()->ADCOffset();

  const int nx = event_data[0].size();
  const double xmin = 0.0;
  const double xmax = (1.0/sample_frequency) * nx;

  std::vector<TH1D*> histograms(num_channels);
  std::vector<std::string> titles = {"PMT", "TPC1", "TPC2", "TPC3"};
  for (int i=0; i<num_channels; i++) {
    histograms[i] = new TH1D(titles[i].c_str(), titles[i].c_str(), event_data[i].size(), xmin, xmax);
  }
  for (int i=0; i<num_channels; i++) {
    const int num_bins = event_data[i].size();
    for (int j=0; j<num_bins; j++) {
      const double v = ((static_cast<double>(event_data[i][j]) * adc_range[i]) / 65536.0 + adc_offset[i]) * 1E3;
      histograms[i]->SetBinContent(j+1, v);
    }
  }

  TCanvas* canv1 = new TCanvas("c1", "c1", 1600, 1200);
  TCanvas* canv2 = new TCanvas("c2", "c2", 1600, 1200);
  canv1 -> Divide(2, 2);
  const double ymin = -100.0;
  const double ymax = 100.0;
  
  for (int i=0; i<num_channels; i++) {
    canv1->cd(i+1);
    histograms[i]->Draw("hist");
    if (i==0) {
      histograms[i]->GetYaxis()->SetRangeUser(-2000.0, 100.0);
    }
    else {
      histograms[i]->GetYaxis()->SetRangeUser(ymin, ymax);
    }
    histograms[i]->GetXaxis()->SetTitle("Time (us)");
    histograms[i]->GetYaxis()->SetTitle("Voltage (mV)");
    histograms[i]->GetXaxis()->SetTitleOffset(0.0);
    histograms[i]->GetXaxis()->CenterTitle();
    histograms[i]->GetYaxis()->SetTitleOffset(0.8);
    histograms[i]->GetYaxis()->CenterTitle();
  }
  gStyle->SetOptStat(0);
  gStyle->SetPalette(56);
  canv1->SaveAs(wfImageName_.c_str());
  image_filenames.push_back(wfImageName_);

  for (int i=0; i<num_channels; i++) {
    canv1->cd(i+1);
    histograms[i]->Draw("hist");
    const double vmin = histograms[i]->GetBinContent(histograms[i]->GetMinimumBin());
    const double vmax = histograms[i]->GetBinContent(histograms[i]->GetMaximumBin());
    const double ymin = vmin - (vmax-vmin)/8.0;
    const double ymax = vmax + (vmax-vmin)/8.0;
    histograms[i]->GetYaxis()->SetRangeUser(ymin, ymax);
  }
  canv1->SaveAs(wfAutorangeImageName_.c_str());
  image_filenames.push_back(wfAutorangeImageName_);

  {
    canv2->cd();
    histograms[0]->Draw("hist");
    //const double vmin = histograms[0]->GetMinimum();
    //const double vmax = histograms[0]->GetMaximum();
    //const double ymin = vmin - (vmax-vmin)/8.0;
    //const double ymax = vmax + (vmax-vmin)/8.0;
    histograms[0]->GetYaxis()->SetRangeUser(ymin, ymax);
    histograms[0]->GetXaxis()->SetRangeUser(5.0, 20.0);
    histograms[0]->GetXaxis()->SetTitle("Time (us)");
    histograms[0]->GetYaxis()->SetTitle("Voltage (mV)");
  }
  canv2->SaveAs(pmtImageName_.c_str());
  image_filenames.push_back(pmtImageName_);

  
  for (int i=0; i<num_channels; i++) {
    delete(histograms[i]);
  }
  delete(canv1);
  delete(canv2);
}

} // namespace gramsballoon
