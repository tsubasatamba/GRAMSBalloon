#ifndef GRAMSBalloon_SocketSessionForReader_hh
#define GRAMSBalloon_SocketSessionForReader_hh 1
#include "SocketSession.hh"
namespace gramsballoon::pgrams {
class SocketSessionForReader: public SocketSession {
public:
  SocketSessionForReader(boost::asio::ip::tcp::socket &&socket) : SocketSession(std::move(socket)) {};
  virtual ~SocketSessionForReader() = default;
  //void start() override;

  virtual void sendAck() {
    requestSend(&bytesReceived_, sizeof(bytesReceived_));
  }

protected:
  void on_receive(const boost::system::error_code &error, std::size_t bytes_transferred) override;

private:
  size_t bytesReceived_ = 0;
};
} // namespace gramsballoon::pgrams
#endif // GRAMSBalloon_SocketSessionForReader_hh