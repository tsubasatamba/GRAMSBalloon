/**
 * Module for Sending telemetry.
 *
 * @author Tsubasa Tamba, Shota Arai
 * @date 2023-03-25
 * @date 2025-**-** | v2.0
 * @date 2025-09-20 | Json-wrapped telemetry. (v3.0)
 */

#ifndef SendTelemetry_H
#define SendTelemetry_H 1

#include "BaseTelemetryDefinition.hh"
#include "ErrorManager.hh"
#include "GetRaspiStatus.hh"
#include "HubHKTelemetry.hh"
#include "MeasureOrientationByMHADC.hh"
#include "MosquittoManager.hh"
#include "ReceiveCommand.hh"
#include "RunIDManager.hh"
#include "SerialCommunication.hh"
#include <anlnext/BasicModule.hh>
#include <thread>

namespace gramsballoon {

class MeasureTemperatureWithRTDSensor;
class ReceiveCommand;
class RunIDManager;
} // namespace gramsballoon
namespace gramsballoon::pgrams {
class GetCompressorData;
class GetArduinoData;
class MeasureTemperatureWithRTDSensorByMHADC;
class MeasureTemperatureWithRTDSensorByArduino;
class GetMHADCData;
class GetPressure;
template <typename T>
class MosquittoManager;
class DistributeCommand;
class MeasureOrientationByMHADC;
class BaseTelemetryDefinition;
class HubHKtelemetry;
class ErrorManager;

class SendTelemetry: public anlnext::BasicModule {
  DEFINE_ANL_MODULE(SendTelemetry, 3.0);
  ENABLE_PARALLEL_RUN();

public:
  SendTelemetry();
  virtual ~SendTelemetry();

protected:
  SendTelemetry(const SendTelemetry &r) = default;

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_finalize() override;

  void writeTelemetryToFile(bool failed);

  void setTelemetryType(int v) { singleton_self()->telemetryType_ = v; }
  int TelemetryType() { return singleton_self()->telemetryType_; }

  ErrorManager *getErrorManager() { return (singleton_self()->errorManager_).get(); }
  int WfDivisionCounter() { return singleton_self()->wfDivisionCounter_; }

private:
  std::shared_ptr<HubHKTelemetry> telemdef_ = nullptr;
  int telemetryType_ = 1;
  std::shared_ptr<ErrorManager> errorManager_ = nullptr;
  int fileEventCnt_ = 0;
  int fileIndex_ = 0;
  bool saveTelemetry_ = true;
  std::string binaryFilenameBase_ = "";
  int numTelemPerFile_ = 1000;
  int sleepms_ = 500;
  int wfDivisionCounter_ = 0;
  int chatter_ = 0;
  std::ofstream *telemetryFile_ = nullptr;

  // access to other classes
  ReceiveCommand *receiveCommand_ = nullptr;
  RunIDManager *runIDManager_ = nullptr;
  pgrams::MosquittoManager<std::string> *mosquittoManager_ = nullptr;

  pgrams::MosquittoIO<std::string> *mosq_ = nullptr;
  std::vector<std::string> pubTopics_ = {"telemetry", "telemetry_2"};
  int qos_ = 0;
};

} /* namespace gramsballoon::pgrams */

#endif /* SendTelemetry_H */
