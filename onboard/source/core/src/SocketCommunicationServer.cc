#include "SocketCommunicationServer.hh"
#include <iostream>
namespace gramsballoon::pgrams {
void SigPipeHander(int) {
  std::cout << "Caught SIGPIPE!" << std::endl;
}
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
void SocketCommunication::accept() {
  acceptor_->async_accept([this](const boost::system::error_code &error, boost::asio::ip::tcp::socket socket) {
    if (!error) {
      socketsAccepted_.push_back(std::make_shared<SocketSession>(std::move(socket)));
    }
    else {
      failed_->store(true, std::memory_order_release);
      std::cerr << "Error in SocketCommunication: Accept failed. Error Code: " << error.message() << std::endl;
    }
  });
}
void SocketCommunication::HandleSIGPIPE() {
  struct sigaction sa;
  sa.sa_handler = SigPipeHander;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGPIPE, &sa, NULL);
}
} // namespace gramsballoon::pgrams