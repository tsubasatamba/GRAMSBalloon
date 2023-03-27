#include "SendTelemetry.hh"


using namespace anlnext;

SendTelemetry::SendTelemetry()
{
  telemgen_ = std::make_unique<TelemetryGenerator>();
  serialPath_ = "/dev/null";
  baudrate_ = B9600;
  openMode_ = O_RDWR;
  readWaveformModuleName_ = "ReadWaveform";
}

SendTelemetry::~SendTelemetry() = default;

ANLStatus SendTelemetry::mod_define()
{
  define_parameter("GetEnvironmentalData_module_names", &mod_class::getEnvironmentalDataModuleNames_);
  define_parameter("MasureTemperature_module_names", &mod_class::measureTemperatureModuleNames_);
  define_parameter("ReadWaveform_module_name", &mod_class::readWaveform_);
  
  define_parameter("serial_path", &mod_class::serialPath_);
  define_parameter("baudrate", &mod_class::baudrate_);
  define_parameter("open_mode", &mod_class::openMode_);
  return AS_OK;
}

ANLStatus SendTelemetry::mod_initialize()
{
  const int num_modules_env = getEnvironmentalDataModuleNames_.size();
  for (int i=0; i<num_modules_env; i++) {
    const std::string module_name = getEnvironmentalDataModuleNames_[i];
    if (exist_module(module_name)) {
      GetEnvironmentalData* ged;
      get_module_NC(module_name, &ged);
      BME680IO* io = ged -> GetBME680IO();
      telemgen_ -> addBME680IO(io);
    }
  }

  if (exist_module(readWaveformModuleName_)) {
    get_module_NC(readWaveformModuleName_, &readWaveform_);
    DAQIO* io = readWaveform_->getDAQIO();
    telemgen_->setDAQIO(io);
  }

  const int num_modules_temp = measureTemperatureModuleNames_.size();
  for (int i=0; i<num_modules_temp; i++) {
    const std::string module_name = measureTemperatureModuleNames_[i];
    if (exist_module(module_name)) {
      MeasureTemperatureWithRTDSensor* mt;
      get_module_NC(module_name, &mt);
      MAX31865IO* io = mt -> GetMAX31865IO();
      telemgen_ -> addMAX31865IO(io);
    }
  }

  // communication
  sc_ = std::make_unique<SerialCommunication>(serialPath_, baudrate_, openMode_);
  sc_->initialize();
  
  return AS_OK;
}

ANLStatus SendTelemetry::mod_analyze()
{
  telemgen_->generateTelemetry(telemetryType_);

  if (telemetryType_==2) {
    telemetryType_ = 1;
  }

  const std::vector<uint8_t>& telemetry = telemgen_->Telemetry();
  const int status = sc_->swrite(telemetry);
  if (status != static_cast<int>(telemetry.size())) {
    std::cerr << "Sending telemetry failed: status = " << status << std::endl;
  }

  std::cout << (int)telemetry.size() << std::endl;

  //debug

  #if 1
  
  // std::vector<char> dbg(telemetry.size()*2+1);
  // for (int i=0; i<(int)telemetry.size(); i++) {
  //   sprintf(&dbg[i * 2], "%02x", (unsigned int)telemetry[i]);
  // }
  // printf("md5 digest: %s\n", &dbg[0]);

  for (int i=0; i<(int)telemetry.size(); i++) {
    std::cout << static_cast<int>(telemetry[i]) << std::endl;
  }

  #endif
  
  return AS_OK;
}

ANLStatus SendTelemetry::mod_finalize()
{
  return AS_OK;
}

