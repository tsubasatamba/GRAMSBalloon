#include <sstream>
#include "ReadWaveform.hh"


using namespace anlnext;

ReadWaveform::ReadWaveform()
{
  daqio_ = std::make_unique<DAQIO>();
  adcRangeList_ = std::vector<double>(4, 1.0);
  adcOffsetList_ = std::vector<double>(4, 0.0);
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
    std::cerr << "Trigger setup failed." << std::endl;
    return AS_QUIT_ERROR;
  }
  
  return AS_OK;
}

ANLStatus ReadWaveform::mod_analyze()
{
  const int event_id = get_loop_index();
  if (event_id%numEventsPerFile_ == 0) {
    if (event_id != 0) {
      closeOutputFile();
    }
    createNewOutputFile();
  }

  daqio_->getData(event_id, eventHeader_, eventData_);
  if (ondemand_) {
    TelemetryGenerator* telemgen = sendTelemetry_->GetTelemetryGenerator();
    telemgen->setEventID(event_id);
    telemgen->setEventHeader(eventHeader_);
    telemgen->setEventData(eventData_);
    sendTelemetry_->setTelemetryType(2);
    ondemand_ = false;
  }
  writeData();

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
  ofs_ = std::ofstream(filename, std::ios::out|std::ios::binary);
  fileID_++;

  std::vector<short> file_header;
  daqio_->generateFileHeader(file_header, numEventsPerFile_);
  const int size = sizeof(short) * static_cast<int>(file_header.size());
  ofs_.write(reinterpret_cast<char*>(&file_header[0]), size);
}

void ReadWaveform::closeOutputFile()
{
  std::vector<short> file_footer;
  daqio_->generateFileFooter(file_footer);
  const int size = sizeof(short) * static_cast<int>(file_footer.size());
  ofs_.write(reinterpret_cast<char*>(&file_footer[0]), size);
  ofs_.close();
}

void ReadWaveform::writeData()
{
  const int header_size = sizeof(short) * static_cast<int>(eventHeader_.size());
  ofs_.write(reinterpret_cast<char*>(&eventHeader_[0]), header_size);

  for (int i=0; i<static_cast<int>(eventData_.size()); i++) {
    const int data_size = sizeof(short) * static_cast<int>(eventData_[i].size());
    ofs_.write(reinterpret_cast<char*>(&eventData_[i][0]), data_size);
  }
}
