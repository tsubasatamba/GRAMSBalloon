#include "SocketCommunicationServer.hh"
#include <iostream>
namespace gramsballoon::pgrams {
SocketCommunication::SocketCommunication(int port) {
  ioContext_ = std::make_shared<boost::asio::io_context>();
  socket_ = std::make_shared<boost::asio::ip::tcp::socket>(*ioContext_);
  acceptor_ = std::make_shared<boost::asio::ip::tcp::acceptor>(
      *ioContext_, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port));
}
SocketCommunication::~SocketCommunication() {
  if (socket_) {
    socket_->close();
  }
  if (acceptor_) {
    acceptor_->close();
  }
  if (ioContext_) {
    ioContext_->stop();
  }
}
int SocketCommunication::accept() {
  acceptor_->async_accept([this](const boost::system::error_code &error, boost::asio::ip::tcp::socket socket) {
    if (!error) {
      socketAccepted_ = std::make_shared<boost::asio::ip::tcp::socket>(std::move(socket));
    }
    else {
      failed_ = true;
      std::cerr << "Error in SocketCommunication: Accept failed. Error Code: " << error.message() << std::endl;
    }
  });
  return 0;
}
void on_send(const boost::system::error_code &error, std::size_t bytes_transferred, int *return_code) {
  if (error) {
    std::cerr << "Error in SocketCommunication: Send failed. " << error.message() << std::endl;
    *return_code = -1;
  }
  else {
    std::cout << "Sent " << bytes_transferred << " bytes." << std::endl;
    *return_code = bytes_transferred;
  }
}
void on_receive(const boost::system::error_code &error, std::size_t bytes_transferred) {
  if (error) {
    std::cerr << "Error in SocketCommunication: Receive failed. " << error.message() << std::endl;
  }
  else {
    std::cout << "Received " << bytes_transferred << " bytes." << std::endl;
  }
}
} // namespace gramsballoon::pgrams