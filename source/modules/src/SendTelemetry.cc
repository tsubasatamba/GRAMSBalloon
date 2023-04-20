#include "SendTelemetry.hh"

using namespace anlnext;

namespace gramsballoon {

SendTelemetry::SendTelemetry()
{
  telemdef_ = std::make_shared<TelemetryDefinition>();
  serialPath_ = "/dev/null";
  baudrate_ = B9600;
  openMode_ = O_RDWR;
  readWaveformModuleName_ = "ReadWaveform";
}

SendTelemetry::~SendTelemetry() = default;

ANLStatus SendTelemetry::mod_define()
{
  define_parameter("GetEnvironmentalData_module_names", &mod_class::getEnvironmentalDataModuleNames_);
  define_parameter("MeasureTemperature_module_names", &mod_class::measureTemperatureModuleNames_);
  define_parameter("ReadWaveform_module_name", &mod_class::readWaveformModuleName_);
  
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
      getEnvironmentalDataVec_.push_back(ged);
    }
  }
  int n = getEnvironmentalDataVec_.size();
  telemdef_->resizeEnvTemperature(n);
  telemdef_->resizeEnvHumidity(n);
  telemdef_->resizeEnvPressure(n);

  if (exist_module(readWaveformModuleName_)) {
    get_module_NC(readWaveformModuleName_, &readWaveform_);
  }

  const int num_modules_temp = measureTemperatureModuleNames_.size();
  for (int i=0; i<num_modules_temp; i++) {
    const std::string module_name = measureTemperatureModuleNames_[i];
    if (exist_module(module_name)) {
      MeasureTemperatureWithRTDSensor* mt;
      get_module_NC(module_name, &mt);
      measureTemperatureVec_.push_back(mt);
    }
  }
  n = measureTemperatureVec_.size();
  telemdef_->resizeRTDTemperatureADC(n);

  if (exist_module("ReceiveCommand")) {
    get_module_NC("ReceiveCommand", &receiveCommand_);
  }


  // communication
  sc_ = std::make_shared<SerialCommunication>(serialPath_, baudrate_, openMode_);
  sc_->initialize();
  
  return AS_OK;
}

ANLStatus SendTelemetry::mod_analyze()
{
  inputInfo();

  telemdef_->setTelemetryType(telemetryType_);
  telemdef_->generateTelemetry();

  if (telemetryType_==2) {
    telemetryType_ = 1;
  }

  const std::vector<uint8_t>& telemetry = telemdef_->Telemetry();
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
    std::cout << i << " " << static_cast<int>(telemetry[i]) << std::endl;
  }

  #endif

  std::this_thread::sleep_for(std::chrono::seconds(1));
  return AS_OK;
}

ANLStatus SendTelemetry::mod_finalize()
{
  return AS_OK;
}

void SendTelemetry::inputInfo()
{
  inputDetectorInfo();
  inputEnvironmentalData();
  inputTemperatureData();
  inputLastCommandInfo();
}

void SendTelemetry::inputDetectorInfo()
{
  uint32_t event_count = readWaveform_->EventCount();
  telemdef_->setEventCount(event_count);
}

void SendTelemetry::inputEnvironmentalData()
{
  const int n = getEnvironmentalDataVec_.size();
  
  for (int i=0; i<n; i++) {
    telemdef_->setEnvTemperature(i, getEnvironmentalDataVec_[i] -> Temperature());
    telemdef_->setEnvHumidity(i, getEnvironmentalDataVec_[i] -> Humidity());
    telemdef_->setEnvPressure(i, getEnvironmentalDataVec_[i] -> Pressure());
  }
}

void SendTelemetry::inputTemperatureData()
{
  const int n = measureTemperatureVec_.size();
  for (int i=0; i<n; i++) {
    telemdef_->setRTDTemperatureADC(i, measureTemperatureVec_[i]->TemperatureADC());
  }
}

void SendTelemetry::inputLastCommandInfo()
{
  const uint16_t last_command_code = receiveCommand_ -> LastCommandCode();
  telemdef_->setLastCommandCode(last_command_code);
  std::cout << "last_command_code: " << last_command_code << std::endl;
}

} /* namespace gramsballoon */
