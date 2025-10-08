#ifndef GB_MosquittoManager_hh
#define GB_MosquittoManager_hh 1
#include "ErrorManager.hh"
#include "MosquittoIO.hh"
#include "SendTelemetry.hh"
#include "anlnext/BasicModule.hh"
#include <string>

namespace gramsballoon {
class SendTelemetry;
} // namespace gramsballoon
namespace gramsballoon::pgrams {
/**
 * Module for managing Mosquitto
 * @author Shota Arai
 * @date 2024-**-** Shota Arai| First implementation.
 * @date 2025-09-20 Shota Arai| Changed to template class to handle different types of telemetry. (v2.0)
**/  

template <typename T>
class MosquittoIO;
template <typename TelemType>
class MosquittoManager: public anlnext::BasicModule {
  DEFINE_ANL_MODULE(MosquittoManager<TelemType>, 2.0);
  ENABLE_PARALLEL_RUN();

public:
  MosquittoManager() = default;
  virtual ~MosquittoManager() = default;

protected:
  MosquittoManager(const MosquittoManager<TelemType> &r) = default;

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_pre_initialize() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_begin_run() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_end_run() override;
  anlnext::ANLStatus mod_finalize() override;

  void Publish(TelemType &message, const std::string &topic, int qos) {
    singleton_self()->mosquittoIO_->Publish(message, topic, qos);
  }
  MosquittoIO<TelemType> *getMosquittoIO() { return singleton_self()->mosquittoIO_.get(); }

private:
  std::shared_ptr<MosquittoIO<TelemType>> mosquittoIO_ = nullptr;
  std::string host_ = "localhost";
  std::string user_ = "";
  std::string passwd_ = "";
  std::string deviceId_ = "MosquittoManager";
  SendTelemetry *sendTelemetry_ = nullptr;
  int port_ = 1883;
  int keepAlive_ = 60;
  int chatter_ = 0;
  int timeout_ = 10;
  bool threadedSet_ = true;
  anlnext::ANLStatus HandleError(int error_code);
};
} // namespace gramsballoon::pgrams
#endif // GB_MosquittoManager_hh