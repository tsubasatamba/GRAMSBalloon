#ifndef GB_SocketCommunicationServer_hh
#define GB_SocketCommunicationServer_hh 1

#include "boost/asio.hpp"
#include "boost/asio/steady_timer.hpp"
#include "boost/bind/bind.hpp"
#include <atomic>
#include <iostream>
#include <memory>
#include <vector>
#include <optional>
namespace gramsballoon::pgrams {
typedef boost::asio::detail::socket_option::integer<SOL_SOCKET, SO_RCVTIMEO> rcv_timeout_option;
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
  SocketCommunication(int port);
  SocketCommunication(std::shared_ptr<boost::asio::io_context> ioContext, int port);
  virtual ~SocketCommunication();

protected:
  SocketCommunication(const SocketCommunication &r) = default;

private:
  std::shared_ptr<boost::asio::io_context> ioContext_ = nullptr;
  std::shared_ptr<boost::asio::ip::tcp::socket> socket_ = nullptr;
  std::shared_ptr<boost::asio::ip::tcp::socket> socketAccepted_;
  std::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor_ = nullptr;
  std::shared_ptr<std::atomic<bool>> failed_ = nullptr;
  std::shared_ptr<std::atomic<bool>> stopped_ = nullptr;
  std::shared_ptr<std::mutex> sockMutex_ = nullptr;
  std::optional<int> timeout_ = 200;

public:
  void accept();
  void setTimeout(int timeout_ms) {
    timeout_ = timeout_ms;
  }
  std::optional<int> getTimeout() const {
    return timeout_;
  }
  bool isConnected() const {
    std::lock_guard<std::mutex> lock(*sockMutex_);
    return socketAccepted_ && socketAccepted_->is_open();
  }
  bool isFailed() const {
    return failed_->load(std::memory_order_acquire);
  }
  void resetFailed() {
    failed_->store(false, std::memory_order_release);
  }
  int close() {
    std::lock_guard<std::mutex> lock(*sockMutex_);
    if (socketAccepted_) {
      socketAccepted_->close();
      socketAccepted_.reset();
    }
    socket_->close();
    stopped_->store(true, std::memory_order_release);
    if (ioContext_) {
      ioContext_->stop();
    }
    return 0;
  }
  std::string getIP() const {
    std::lock_guard<std::mutex> lock(*sockMutex_);
    return socketAccepted_->remote_endpoint().address().to_string();
  }
  int getPort() const {
    std::lock_guard<std::mutex> lock(*sockMutex_);
    return socketAccepted_->remote_endpoint().port();
  }
  bool isOpened() const {
    std::lock_guard<std::mutex> lock(*sockMutex_);
    return socket_->is_open();
  }
  int getSocket() const {
    std::lock_guard<std::mutex> lock(*sockMutex_);
    return socket_->native_handle();
  }
  static void HandleSIGPIPE();
  int send(const void *buf, size_t n) {
    std::lock_guard<std::mutex> lock(*sockMutex_);
    if (socketAccepted_) {
      boost::system::error_code errorcode;
      {
        const auto ret = boost::asio::write(*socketAccepted_, boost::asio::buffer(buf, n), errorcode);
        if (errorcode) {
          std::cerr << "Error in SocketCommunication: " << errorcode.message() << std::endl;
          return errorcode.value();
        }
        if (ret == 0) {
          std::cerr << "Error in SocketCommunication: No data sent." << std::endl;
          return -1;
        }
        return ret;
      }
    }
    return -1;
  }
  template <typename T>
  int send(const std::vector<T> &data) {
    return send(&data[0], sizeof(T) * data.size());
  }
  template <typename T>
  int receive(std::vector<T> &data) {
    std::lock_guard<std::mutex> lock(*sockMutex_);
    if (socketAccepted_) {
      boost::system::error_code errorcode;
      size_t sz = socketAccepted_->available(errorcode);
      if (errorcode) {
        std::cerr << "Error in SocketCommunication: " << errorcode.message() << std::endl;
        return errorcode.value();
      }
      if (sz == 0) {
        return 0;
      }
      const auto n = sz / sizeof(T);
      data.resize(n);
      return boost::asio::read(*socketAccepted_, boost::asio::buffer(&data[0], n), errorcode);
    }
    return -1;
  }
};

} // namespace gramsballoon::pgrams
#endif //GB_SocketCommunicationServer_hh