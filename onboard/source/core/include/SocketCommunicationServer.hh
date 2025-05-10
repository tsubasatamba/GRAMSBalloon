#ifndef GB_SocketCommunicationServer_hh
#define GB_SocketCommunicationServer_hh 1

#include "boost/asio.hpp"
#include "boost/bind.hpp"
#include <iostream>
#include <memory>
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
  std::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor_ = nullptr;
  bool failed_ = false;

public:
  int accept();
  bool isFailed() const {
    return failed_;
  }
  void on_accept(const boost::system::error_code &error) {
    if (!error) {
      std::cout << "SocketCommunication: Accepted connection." << std::endl;
    }
    else {
      std::cerr << "Error in SocketCommunication: Accept failed." << std::endl;
    }
  }
  int close() {
    socket_->close();
    return 0;
  }
  int send(const void *buf, size_t n) {
    boost::system::error_code error_code;
    const int num_bytes = socket_->send(boost::asio::buffer(buf, n), 0, error_code);
    if (error_code) {
      std::cerr << "Error in SocketCommunication: Send failed. " << error_code.message() << std::endl;
      return -1;
    }
    return num_bytes;
  }
  template <typename T>
  int send(const std::vector<T> &data) {
    return send(data.data(), data.size() * sizeof(T));
  }
  template <typename T>
  int receive(const std::vector<T> &data) {
    return receive(data.data(), data.size() * sizeof(T));
  }
  int receive(void *buf, size_t n) {
    boost::system::error_code error_code;
    const int num_bytes = socket_->receive(boost::asio::buffer(buf, n), 0, error_code);
    if (error_code) {
      std::cerr << "Error in SocketCommunication: Receive failed. " << error_code.message() << std::endl;
      return -1;
    }
    return num_bytes;
  }
  int getSocket() {
    return socket_->native_handle();
  }
  boost::asio::ip::tcp::socket &getSocketRef() {
    return *socket_;
  }
};
} // namespace gramsballoon::pgrams
#endif //GB_SocketCommunicationServer_hh