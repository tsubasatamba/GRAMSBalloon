#ifndef GRAMSBalloon_SocketSessionForWriter_hh
#define GRAMSBalloon_SocketSessionForWriter_hh 1
#include "SocketSession.hh"
namespace gramsballoon::pgrams {
class SocketSessionForWriter: public SocketSession {
public:
  SocketSessionForWriter(boost::asio::ip::tcp::socket &&socket) : SocketSession(std::move(socket)) {};
  virtual ~SocketSessionForWriter() = default;
  //void start() override;
  //void on_send(const boost::system::error_code &error, std::size_t bytes_transferred) override;

private:
  size_t bytesSent_ = 0;
};
} // namespace gramsballoon::pgrams
#endif // GRAMSBalloon_SocketSessionForWriter_hh