#ifndef GB_SocketCommunicationServer_hh
#define GB_SocketCommunicationServer_hh 1

#include "SocketSession.hh"
#include "SocketSessionForReader.hh"
#include "SocketSessionForWriter.hh"
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
class SocketCommunication: public std::enable_shared_from_this<SocketCommunication> {
public:
  SocketCommunication(int port, bool isWriter = false);
  virtual ~SocketCommunication();

protected:
  SocketCommunication(const SocketCommunication &r) = default;

private:
  std::shared_ptr<boost::asio::io_context> ioContext_ = nullptr;
  std::shared_ptr<boost::asio::ip::tcp::socket> socket_ = nullptr;
  std::vector<std::shared_ptr<SocketSession>> socketsAccepted_;
  std::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor_ = nullptr;
  std::shared_ptr<std::atomic<int>> failedCount_ = nullptr;
  std::shared_ptr<std::atomic<bool>> stopped_ = nullptr;
  std::deque<std::vector<uint8_t>> receiveQueue_;
  bool isWriter_ = false;

public:
  void accept();
  void run() {
    if (ioContext_) {
      stopped_->store(false, std::memory_order_release);
      ioContext_->run();
    }
  }
  bool isFailed() const {
    return failedCount_->load(std::memory_order_acquire) != 0;
  }
  void resetFailed() {
    failedCount_->store(0, std::memory_order_release);
  }
  int close() {
    for (auto sock: socketsAccepted_) {
      if (sock == nullptr) {
        continue;
      }
      sock->stop();
      sock.reset();
    }
    socketsAccepted_.clear();
    socket_->close();
    stopped_->store(true, std::memory_order_release);
    if (ioContext_) {
      ioContext_->stop();
    }
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
        continue;
      }
      sock->requestSend(buf, n);
    }
  }
  int checkSocketStatus() {
    for (auto sock: socketsAccepted_) {
      if (sock == nullptr) {
        continue;
      }
      if (sock->getStopFlag()) {
        sock->stop();
        std::cerr << "Socket is closed." << std::endl;
        sock.reset();
      }
      if (sock->isFailed()) {
        std::cerr << "Error in SocketCommunication::checkSocketStatus: Socket is failed." << std::endl;
        failedCount_->fetch_add(1, std::memory_order_acq_rel);
        sock->stop();
        sock.reset();
        std::cerr << "Socket is closed." << std::endl;
      }
    }
    return failedCount_->load(std::memory_order_acquire);
  }
  void requestStop() {
    for (auto sock: socketsAccepted_) {
      if (sock == nullptr) {
        continue;
      }
      sock->stop();
    }
    stopped_->store(true, std::memory_order_release);
  }
  template <typename T>
  void send(const std::vector<T> &data) {
    return send(&data[0], sizeof(T) * data.size());
  }
  void updateBuffer() {
    for (auto sock: socketsAccepted_) {
      if (sock == nullptr) {
        continue;
      }
      if (sock->isBufferEmpty()) continue;
      receiveQueue_.push_back(sock->popBuffer());
    }
  }
  int receive(std::vector<uint8_t> &&data) {
    if (receiveQueue_.empty()) {
      return 0;
    }
    data = std::move(receiveQueue_.front());
    receiveQueue_.pop_front();
    return data.size();
  }
};

} // namespace gramsballoon::pgrams
#endif //GB_SocketCommunicationServer_hh