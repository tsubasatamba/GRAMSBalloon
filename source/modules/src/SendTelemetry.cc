#include "SendTelemetry.hh"

using namespace anlnext;

namespace gramsballoon {

SendTelemetry::SendTelemetry()
{
  telemdef_ = std::make_shared<TelemetryDefinition>();
  errorManager_ = std::make_shared<ErrorManager>();
  TPCHVControllerModuleName_ = "ControlHighVoltage_TPC";
  PMTHVControllerModuleName_ = "ControlHighVoltage_PMT";
  serialPath_ = "/dev/null";
  baudrate_ = B9600;
  openMode_ = O_RDWR;
}

SendTelemetry::~SendTelemetry() = default;

ANLStatus SendTelemetry::mod_define()
{
  define_parameter("MeasureTemperature_module_names", &mod_class::measureTemperatureModuleNames_);
  define_parameter("TPCHVController_module_name", &mod_class::TPCHVControllerModuleName_);
  define_parameter("PMTHVController_module_name", &mod_class::PMTHVControllerModuleName_);
  define_parameter("GetEnvironmentalData_module_names", &mod_class::getEnvironmentalDataModuleNames_);
  
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

  if (exist_module(TPCHVControllerModuleName_)) {
    get_module_NC(TPCHVControllerModuleName_, &TPCHVController_);
  }

  if (exist_module(PMTHVControllerModuleName_)) {
    get_module_NC(PMTHVControllerModuleName_, &PMTHVController_);
  }

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

  const std::string get_slowADC_data_md = "GetSlowADCData";
  if (exist_module(get_slowADC_data_md)) {
    get_module_NC(get_slowADC_data_md, &getSlowADCData_);
  }

  const std::string receive_command_md = "ReceiveCommand";
  if (exist_module(receive_command_md)) {
    get_module_NC(receive_command_md, &receiveCommand_);
  }

  // communication
  sc_ = std::make_shared<SerialCommunication>(serialPath_, baudrate_, openMode_);
  const int status = sc_ -> initialize();
  if (status!=0) {
    std::cerr << "Error in SendTelemetry::mod_initialize: Serial communication failed." << std::endl;
    return AS_QUIT_ERROR;
  }
  
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
    inputSoftwareInfo();
  }
  else if (telemetryType_==2) {
    ;
  }
  else if (telemetryType_==3) {
    inputStatusInfo();
  }
  else {
    std::cerr << "Error in SendTelemetry::inputInfo(): wrong telemetry type " << telemetryType_ << std::endl;
  }
}

void SendTelemetry::inputDetectorInfo()
{
  if (readWaveform_!=nullptr) {
    uint32_t event_count = readWaveform_->EventCount();
    telemdef_->setEventCount(event_count);
  }
  // trigger count
  if (getSlowADCData_!=nullptr) {
    telemdef_->setChamberPressure(getSlowADCData_->getADC(2));
  }
  const int n = measureTemperatureVec_.size();
  for (int i=0; i<n; i++) {
    telemdef_->setRTDTemperatureADC(i, measureTemperatureVec_[i]->TemperatureADC());
  }

  if (TPCHVController_!=nullptr) {
    telemdef_->setTPCHVSetting(TPCHVController_->NextVoltage());
  }
  if (getSlowADCData_!=nullptr) {
    telemdef_->setTPCHVMeasure(getSlowADCData_->getADC(3));
  }
  if (PMTHVController_!=nullptr) {
    telemdef_->setPMTHVSetting(PMTHVController_->NextVoltage());
  }
  if (getSlowADCData_!=nullptr) {
    telemdef_->setPMTHVMeasure(getSlowADCData_->getADC(4));
  }
}


void SendTelemetry::inputHKVesselInfo()
{
  if (getRaspiStatus_!=nullptr) {
    telemdef_->setCPUTemperature(getRaspiStatus_->CPUTemperature());
  }

  const int n_env = getEnvironmentalDataVec_.size();
  for (int i=0; i<n_env; i++) {
    telemdef_->setEnvTemperature(i, getEnvironmentalDataVec_[i] -> Temperature());
    telemdef_->setEnvHumidity(i, getEnvironmentalDataVec_[i] -> Humidity());
    telemdef_->setEnvPressure(i, getEnvironmentalDataVec_[i] -> Pressure());
  }

  if (measureAcceleration_!=nullptr) {
    for (int i=0; i<3; i++) {
      telemdef_->setAcceleration(i, measureAcceleration_->getAcceleration(i));
      telemdef_->setGyro(i, measureAcceleration_->getGyro(i));
      telemdef_->setMagnet(i, measureAcceleration_->getMagnet(i));
    }
  }
  if (getSlowADCData_!=nullptr) {
    telemdef_->setMainCurrent(getSlowADCData_->getADC(1));
    telemdef_->setMainVoltage(getSlowADCData_->getADC(0));
  }
}


void SendTelemetry::inputSoftwareInfo()
{
  if (receiveCommand_!=nullptr) {
    telemdef_->setLastCommandIndex(receiveCommand_->CommandIndex());
    telemdef_->setLastCommandCode(receiveCommand_ -> CommandCode());
    telemdef_->setCommandRejectCount(receiveCommand_->CommandRejectCount());
  }
  telemdef_->setSoftwareErrorCode(errorManager_->ErrorCode());
}

void SendTelemetry::inputStatusInfo()
{
  if (readWaveform_!=nullptr) {
    telemdef_->setTriggerMode(static_cast<uint16_t>(readWaveform_->TrigMode()));
    telemdef_->setTriggerDevice(static_cast<uint16_t>(readWaveform_->TrigDevice()));
    telemdef_->setTriggerChannel(static_cast<uint16_t>(readWaveform_->TrigChannel()));
    telemdef_->setTriggerLevel(readWaveform_->TrigLevel());
    telemdef_->setTriggerPosition(readWaveform_->TrigPosition());
    // chmask
    telemdef_->setADCOffset(readWaveform_->Offset());
    telemdef_->setADCRange(readWaveform_->Range());
  }
  if (getRaspiStatus_!=nullptr) {
    telemdef_->setSDCapacity(getRaspiStatus_->CapacityFree());
  }

}

} /* namespace gramsballoon */
