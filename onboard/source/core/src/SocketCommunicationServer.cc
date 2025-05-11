#include "SocketCommunicationServer.hh"
#include <iostream>
namespace gramsballoon::pgrams {
void SigPipeHander(int) {
  std::cout << "Caught SIGPIPE!" << std::endl;
}
SocketCommunication::SocketCommunication(int port, bool isWriter) {
  ioContext_ = std::make_shared<boost::asio::io_context>();
  socket_ = std::make_shared<boost::asio::ip::tcp::socket>(*ioContext_);
  acceptor_ = std::make_shared<boost::asio::ip::tcp::acceptor>(
      *ioContext_, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port));
  failedCount_ = std::make_shared<std::atomic<int>>(0);
  stopped_ = std::make_shared<std::atomic<bool>>(true);
  isWriter_ = isWriter;
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
  auto self = shared_from_this();
  acceptor_->async_accept([this, self](const boost::system::error_code &error, boost::asio::ip::tcp::socket socket) {
    if (!error) {
      if (isWriter_)
        socketsAccepted_.push_back(std::make_shared<SocketSessionForWriter>(std::move(socket)));
      else
        socketsAccepted_.push_back(std::make_shared<SocketSessionForReader>(std::move(socket)));
      socketsAccepted_.back()->start();
    }
    else {
      failedCount_->fetch_add(1, std::memory_order_acq_rel);
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