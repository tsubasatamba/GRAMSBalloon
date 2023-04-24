#include <sstream>
#include "ReadWaveform.hh"

using namespace anlnext;

namespace gramsballoon {

ReadWaveform::ReadWaveform()
{
  ADManagerName_ = "AnalogDiscoveryManager";
  daqio_ = std::make_shared<DAQIO>();
  adcRangeList_ = std::vector<double>(4, 1.0);
  adcOffsetList_ = std::vector<double>(4, 0.0);
  ofs_ = std::make_shared<std::ofstream>();
  outputFilenameBase_ = "output";
}

ReadWaveform::~ReadWaveform() = default;

ANLStatus ReadWaveform::mod_define()
{
  define_parameter("ad_manager_name", &mod_class::ADManagerName_);
  define_parameter("trig_device", &mod_class::trigDevice_);
  define_parameter("trig_channel", &mod_class::trigChannel_);
  define_parameter("trig_mode", &mod_class::trigMode_);
  define_parameter("trig_level", &mod_class::trigLevel_);
  define_parameter("trig_position", &mod_class::trigPosition_);
  define_parameter("time_window", &mod_class::timeWindow_);
  define_parameter("sample_frequency", &mod_class::sampleFreq_);
  define_parameter("adc_range_list", &mod_class::adcRangeList_);
  define_parameter("adc_offset_list", &mod_class::adcOffsetList_);
  define_parameter("output_filename_base", &mod_class::outputFilenameBase_);
  define_parameter("num_events_per_file", &mod_class::numEventsPerFile_);
  define_parameter("start_reading", &mod_class::startReading_);

  return AS_OK;
}

ANLStatus ReadWaveform::mod_initialize()
{
  get_module_NC(ADManagerName_, &ADManager_);
  AnalogDiscoveryIO* adio = ADManager_->ADIO();
  const int num_devices = adio->NumDevices();
  const int num_sample = static_cast<int>(sampleFreq_*timeWindow_);

  get_module_NC("SendTelemetry", &sendTelemetry_);

  int k = 0;
  for (int i=0; i<num_devices; i++) {
    for (int j=0; j<2; j++) {
      adio -> setupAnalogIn(i, j, sampleFreq_*1.0E6, num_sample, adcRangeList_[k], adcOffsetList_[k]);
      k++;
    }
  }

  daqio_->setAnalogDiscoveryIO(adio);
  daqio_->initialize();
  daqio_->setTriggerParameters(trigDevice_, trigChannel_, trigMode_, trigLevel_, trigPosition_);
  daqio_->setSampleParameters(sampleFreq_, timeWindow_);
  const int status = daqio_->setupTrigger();
  if (status!=0) {
    std::cerr << "Trigger setup failed in ReadWaveform::mod_initialize" << std::endl;
    return AS_QUIT_ERROR;
  }
  
  return AS_OK;
}

ANLStatus ReadWaveform::mod_analyze()
{
  if (triggerChanged_) {
    daqio_->setTriggerParameters(trigDevice_, trigChannel_, trigMode_, trigLevel_, trigPosition_);
    const int status = daqio_->setupTrigger();
    if (status!=0) {
      std::cerr << "Trigger setup failed in ReadWaveform::mod_analyze" << std::endl;
      return AS_QUIT_ERROR;
    }
    triggerChanged_ = false;
  }

  if (!startReading_) {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    return AS_OK;
  }

  if (eventID_%numEventsPerFile_ == 0) {
    if (eventID_ != 0) {
      closeOutputFile();
    }
    createNewOutputFile();
  }
  daqio_->getData(eventID_, eventHeader_, eventData_);
  if (ondemand_) {
    sendTelemetry_->setEventID(eventID_);
    sendTelemetry_->setEventHeader(eventHeader_);
    sendTelemetry_->setEventData(eventData_);
    sendTelemetry_->setTelemetryType(2);
    ondemand_ = false;
  }
  writeData();
  evenID_++;

  return AS_OK;
}

ANLStatus ReadWaveform::mod_finalize()
{
  closeOutputFile();
  return AS_OK;
}

void ReadWaveform::createNewOutputFile()
{
  std::ostringstream sout;
  sout << std::setfill('0') << std::right << std::setw(6) << fileID_;
  const std::string id_str = sout.str();
  const std::string filename = outputFilenameBase_ + "_" + id_str + ".dat";
  ofs_ = std::make_shared<std::ofstream>(filename, std::ios::out|std::ios::binary);
  fileID_++;

  std::vector<int16_t> file_header;
  daqio_->generateFileHeader(file_header, numEventsPerFile_);
  const int size = sizeof(int16_t) * static_cast<int>(file_header.size());
  ofs_->write(reinterpret_cast<char*>(&file_header[0]), size);
}

void ReadWaveform::closeOutputFile()
{
  std::vector<int16_t> file_footer;
  daqio_->generateFileFooter(file_footer);
  const int size = sizeof(int16_t) * static_cast<int>(file_footer.size());
  ofs_->write(reinterpret_cast<char*>(&file_footer[0]), size);
  ofs_->close();
}

void ReadWaveform::writeData()
{
  const int header_size = sizeof(int16_t) * static_cast<int>(eventHeader_.size());
  ofs_->write(reinterpret_cast<char*>(&eventHeader_[0]), header_size);

  for (int i=0; i<static_cast<int>(eventData_.size()); i++) {
    const int data_size = sizeof(int16_t) * static_cast<int>(eventData_[i].size());
    ofs_->write(reinterpret_cast<char*>(&eventData_[i][0]), data_size);
  }
}

} /* namespace gramsballoon */
