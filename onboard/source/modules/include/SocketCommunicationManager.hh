/**
 * Module for managing the socket communication.
 *
 * @author Shota Arai
 * @date 2025-03-24 | first draft
 *
 */
#ifndef GRAMSBalloon_SocketCommunicationManager_hh
#define GRAMSBalloon_SocketCommunicationManager_hh 1
#include "SendTelemetry.hh"
#include "SocketCommunicationServer.hh"
#include "anlnext/BasicModule.hh"
#include <optional>
namespace gramsballoon {
class SendTelemetry;
}
namespace gramsballoon::pgrams {
class SocketCommunicationManager: public anlnext::BasicModule {
  DEFINE_ANL_MODULE(SocketCommunicationManager, 1.0);
  ENABLE_PARALLEL_RUN();

public:
  SocketCommunicationManager() = default;
  virtual ~SocketCommunicationManager() = default;

protected:
  SocketCommunicationManager(const SocketCommunicationManager &r) = default;

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_pre_initialize() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_finalize() override;
  SocketCommunication *getSocketCommunication() {
    return socketCommunication_.get();
  }

private:
  std::string ip_;
  int port_;
  int chatter_ = 0;
  std::shared_ptr<SocketCommunication> socketCommunication_ = nullptr;
  SendTelemetry *sendTelemetry_ = nullptr;
  double timeout_ = -1;
  std::optional<timeval> timeoutTV_;
  bool handleSigpipe_ = false;
  std::shared_ptr<std::thread> thread_ = nullptr;
};
} // namespace gramsballoon::pgrams
#endif //GRAMSBalloon_SocketCommunicationManager_hh