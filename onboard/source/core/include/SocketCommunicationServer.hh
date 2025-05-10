#ifndef GB_SocketCommunicationServer_hh
#define GB_SocketCommunicationServer_hh 1

#include "boost/asio.hpp"
#include "boost/asio/steady_timer.hpp"
#include "boost/bind.hpp"
#include <atomic>
#include <iostream>
#include <memory>
#include <vector>
namespace gramsballoon::pgrams {
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
  std::shared_ptr<boost::asio::ip::tcp::socket> socketAccepted_ = nullptr;
  std::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor_ = nullptr;
  std::atomic<bool> failed_ = false;
  boost::asio::steady_timer timer_;
  std::atomic<bool> stopped_ = false;

public:
  int accept();
  bool isFailed() const {
    return failed_;
  }
  int close() {
    socket_->close();
    return 0;
  }
  int send(const void *buf, size_t n) {
    if (socketAccepted_ == nullptr) {
      std::cerr << "Error in SocketCommunication: Socket not accepted." << std::endl;
      return -1;
    }
    int return_code = 0;
    socketAccepted_->async_send(boost::asio::buffer(buf, n), boost::bind(SocketCommunication::on_send, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred()));
  }
  template <typename T>
  int send(const std::vector<T> &data) {
    return send(data.data(), data.size() * sizeof(T));
  }

  void on_send(const boost::system::error_code &error, std::size_t bytes_transferred, int *return_code);
  void on_receive(const boost::system::error_code &error, std::size_t bytes_transferred, int *return_code);

  template <typename T>
  int receive(const std::vector<T> &data) {
    return receive(data.data(), data.size() * sizeof(T));
  }
  int receive(void *buf, size_t n) {
    if (socketAccepted_ == nullptr) {
      std::cerr << "Error in SocketCommunication: Socket not accepted." << std::endl;
      return -1;
    }
    int return_code = 0;
    socketAccepted_->async_receive(boost::asio::buffer(buf, n), boost::bind(SocketCommunication::on_receive, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred()));
    return return_code;
  }
};
} // namespace gramsballoon::pgrams
#endif //GB_SocketCommunicationServer_hh