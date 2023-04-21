#include "SendTelemetry.hh"

using namespace anlnext;

namespace gramsballoon {

SendTelemetry::SendTelemetry()
{
  telemdef_ = std::make_shared<TelemetryDefinition>();
  serialPath_ = "/dev/null";
  baudrate_ = B9600;
  openMode_ = O_RDWR;
}

SendTelemetry::~SendTelemetry() = default;

ANLStatus SendTelemetry::mod_define()
{
  define_parameter("GetEnvironmentalData_module_names", &mod_class::getEnvironmentalDataModuleNames_);
  define_parameter("MeasureTemperature_module_names", &mod_class::measureTemperatureModuleNames_);
  
  define_parameter("serial_path", &mod_class::serialPath_);
  define_parameter("baudrate", &mod_class::baudrate_);
  define_parameter("open_mode", &mod_class::openMode_);
  return AS_OK;
}

ANLStatus SendTelemetry::mod_initialize()
{
  const std::string read_wf_md = "ReadWaveform";
  if (exist_module(read_wf_md)) {
    get_module_NC(read_wf_md, &readWaveform_);
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
  const int ntemp = measureTemperatureVec_.size();
  telemdef_->resizeRTDTemperatureADC(ntemp);

  const std::string get_raspi_status_md = "GetRaspiStatus";
  if (exist_module(get_raspi_status_md)) {
    get_module_NC(get_raspi_status_md, &getRaspiStatus_);
  }

  const int num_modules_env = getEnvironmentalDataModuleNames_.size();
  for (int i=0; i<num_modules_env; i++) {
    const std::string module_name = getEnvironmentalDataModuleNames_[i];
    if (exist_module(module_name)) {
      GetEnvironmentalData* ged;
      get_module_NC(module_name, &ged);
      getEnvironmentalDataVec_.push_back(ged);
    }
  }
  const int nenv = getEnvironmentalDataVec_.size();
  telemdef_->resizeEnvTemperature(nenv);
  telemdef_->resizeEnvHumidity(nenv);
  telemdef_->resizeEnvPressure(nenv);

  const std::string measure_acceleration_md = "MeasureAcceleration";
  if (exist_module(measure_acceleration_md)) {
    get_module_NC(measure_acceleration_md, &measureAcceleration_);
  }

  const std::string receive_command_md = "ReceiveCommand";
  if (exist_module(receive_command_md)) {
    get_module_NC(receive_command_md, &receiveCommand_);
  }

  // communication
  sc_ = std::make_shared<SerialCommunication>(serialPath_, baudrate_, openMode_);
  sc_->initialize();
  
  return AS_OK;
}

ANLStatus SendTelemetry::mod_analyze()
{
  inputInfo();

  telemdef_->generateTelemetry();

  if (telemetryType_==2) {
    telemetryType_ = 1;
  }
  if (telemetryType_==3) {
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
  telemdef_->setTelemetryType(telemetryType_);
  if (telemetryType_==1) {
    inputDetectorInfo();
    inputHKVesselInfo();
    inputLastCommandInfo();
  }
  else if (telemetryType_==2) {
    ;
  }
  else if (telemetryType_==3) {
    ;
  }
  else {
    std::cerr << "Error in SendTelemetry::inputInfo(): wrong telemetry type " << telemetryType_ << std::endl;
  }
}

void SendTelemetry::inputDetectorInfo()
{
  uint32_t event_count = readWaveform_->EventCount();
  telemdef_->setEventCount(event_count);
  // trigger count
  // chamber pressure
  const int n = measureTemperatureVec_.size();
  for (int i=0; i<n; i++) {
    telemdef_->setRTDTemperatureADC(i, measureTemperatureVec_[i]->TemperatureADC());
  }
  // TPCHV Setting
  // TPCHV Measure
  // PMTHV Setting
  // PMTHV Measure
}


void SendTelemetry::inputHKVesselInfo()
{
  telemdef_->setCPUTemperature(getRaspiStatus_->CPUTemperature());

  const int n_env = getEnvironmentalDataVec_.size();
  for (int i=0; i<n_env; i++) {
    telemdef_->setEnvTemperature(i, getEnvironmentalDataVec_[i] -> Temperature());
    telemdef_->setEnvHumidity(i, getEnvironmentalDataVec_[i] -> Humidity());
    telemdef_->setEnvPressure(i, getEnvironmentalDataVec_[i] -> Pressure());
  }

  telemdef_->setAcceleration(measureAcceleration_->getAcceleration());
  telemdef_->setGyro(measureAcceleration_->getGyro());
  telemdef_->setMagnet(measureAcceleration_->getMagnet());
}


void SendTelemetry::inputLastCommandInfo()
{
  const uint16_t last_command_code = receiveCommand_ -> LastCommandCode();
  telemdef_->setLastCommandCode(last_command_code);
  std::cout << "last_command_code: " << last_command_code << std::endl;
}

} /* namespace gramsballoon */
