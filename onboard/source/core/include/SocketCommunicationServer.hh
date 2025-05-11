#ifndef GB_SocketCommunicationServer_hh
#define GB_SocketCommunicationServer_hh 1

#include "SocketSession.hh"
#include "boost/asio.hpp"
#include "boost/asio/steady_timer.hpp"
#include "boost/bind/bind.hpp"
#include <atomic>
#include <iostream>
#include <memory>
#include <vector>
namespace gramsballoon::pgrams {
class SocketSession;
/**
 * @brief SocketCommunicationServer class for managing socket communication.
 *
 * This class provides a basic implementation of a socket communication server.
 * It is designed to be inherited by other classes that require socket communication functionality.
 *
 * @author Shota Arai
 * @date 2025-05-09 | first draft
 */
class SocketCommunication {
public:
  SocketCommunication(int port);
  virtual ~SocketCommunication();

protected:
  SocketCommunication(const SocketCommunication &r) = default;

private:
  std::shared_ptr<boost::asio::io_context> ioContext_ = nullptr;
  std::shared_ptr<boost::asio::ip::tcp::socket> socket_ = nullptr;
  std::vector<std::shared_ptr<SocketSession>> socketsAccepted_;
  std::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor_ = nullptr;
  std::shared_ptr<std::atomic<bool>> failed_ = nullptr;
  std::shared_ptr<std::atomic<bool>> stopped_ = nullptr;

public:
  void accept();
  bool isFailed() const {
    return failed_->load(std::memory_order_acquire);
  }
  int close() {
    socket_->close();
    return 0;
  }
  std::string getIP() const {
    return socket_->remote_endpoint().address().to_string();
  }
  int getPort() const {
    return socket_->remote_endpoint().port();
  }
  bool isOpened() const {
    return socket_->is_open();
  }
  static void HandleSIGPIPE();
  void send(const void *buf, size_t n) {
    for (auto sock: socketsAccepted_) {
      if (sock == nullptr) {
        std::cerr << "Error in SocketCommunication::send: Socket is nullptr." << std::endl;
        continue;
      }
      sock->requestSend(buf, n);
    }
  }
  template <typename T>
  void send(const std::vector<T> &data) {
    return send(&data[0], sizeof(T) * data.size());
  }
};
} // namespace gramsballoon::pgrams
#endif //GB_SocketCommunicationServer_hh