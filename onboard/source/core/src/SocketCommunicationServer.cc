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
  acceptor_->async_accept(*socket_, boost::bind(
                                        &SocketCommunication::on_accept, this, boost::asio::placeholders::error));
  return 0;
}
} // namespace gramsballoon::pgrams