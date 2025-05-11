#include "SocketSessionForReader.hh"
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <iostream>
#include <vector>
namespace gramsballoon::pgrams {
void SocketSessionForReader::start() {
  receiveHandler();
}
void SocketSessionForReader::on_receive(const boost::system::error_code &error, std::size_t bytes_transferred) {
  SocketSession::on_receive(error, bytes_transferred);
  std::cout << "SocketSessionForReader" << std::endl; // For debug
  if (error) {
    return;
  }
  if (bytes_transferred > 0) {
    bytesReceived_ = bytes_transferred;
    sendAck();
    bytesReceived_ = 0;
  }
}
} // namespace gramsballoon::pgrams