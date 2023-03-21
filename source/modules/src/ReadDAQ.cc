#include <sstream>
#include "ReadDAQ.hh"


using namespace anlnext;

ReadDAQ::ReadDAQ()
{
  daqio_ = std::make_unique<DAQIO>();
  adcRangeList_ = std::vector<double>(4, 1.0);
  adcOffsetList_ = std::vector<double>(4, 0.0);
}

ReadDAQ::~ReadDAQ() = default;

ANLStatus ReadDAQ::mod_define()
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

ANLStatus ReadDAQ::mod_initialize()
{
  get_module_NC(ADManagerName_, &ADManager_);
  AnalogDiscoveryIO* adio = ADManager_->ADIO();
  const int num_devices = adio->NumDevices();
  const int num_sample = static_cast<int>(sampleFreq_*timeWindow_);

  int k = 0;
  for (int i=0; i<num_devices; i++) {
    for (int j=0; j<2; j++) {
      adio -> setupAnalogIn(i, j, sampleFreq_, num_sample, adcRangeList_[k], adcOffsetList_[k]);
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

ANLStatus ReadDAQ::mod_analyze()
{
  const int event_id = get_loop_index();
  if (event_id%numEventsPerFile_ == 0) {
    if (event_id != 0) {
      closeOutputFile();
    }
    createNewOutputFile();
  }
  
  daqio_->getData(event_id, eventHeader_, eventData_);
  writeData();

  return AS_OK;
}

ANLStatus ReadDAQ::mod_finalize()
{
  closeOutputFile();
  return AS_OK;
}

void ReadDAQ::createNewOutputFile()
{
  std::ostringstream sout;
  sout << std::setfill('0') << std::right << std::setw(6) << fileID_;
  const std::string id_str = sout.str();
  const std::string filename = outputFilenameBase_ + "_" + id_str + ".dat";
  ofs_ = std::ofstream(filename, std::ios::out|std::ios::binary);
  fileID_++;

  std::vector<short> file_header;
  daqio_->generateFileHeader(file_header, numEventsPerFile_, adcRangeList_, adcOffsetList_);
  const int size = sizeof(short) * static_cast<int>(file_header.size());
  ofs_.write(reinterpret_cast<char*>(&eventHeader_[0]), size);
}

void ReadDAQ::closeOutputFile()
{
  std::vector<short> file_footer;
  daqio_->generateFileFooter(file_footer);
  const int size = sizeof(short) * static_cast<int>(file_footer.size());
  ofs_.write(reinterpret_cast<char*>(&file_footer[0]), size);
}

void ReadDAQ::writeData()
{
  const int header_size = sizeof(short) * static_cast<int>(eventHeader_.size());
  const int data_size = sizeof(short) * static_cast<int>(eventData_.size());
  ofs_.write(reinterpret_cast<char*>(&eventHeader_[0]), header_size);
  ofs_.write(reinterpret_cast<char*>(&eventData_[0]), data_size);
}
