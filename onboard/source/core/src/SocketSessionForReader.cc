#include "SocketSessionForReader.hh"
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <iostream>
#include <vector>
namespace gramsballoon::pgrams {
//void SocketSessionForReader::start() {
//  std::cout << "SocketSessionForReader::start()" << std::endl; // For debug
//  receiveHandler();
//  sendHandler();
//}
void SocketSessionForReader::on_receive(const boost::system::error_code &error, std::size_t bytes_transferred) {
  SocketSession::on_receive(error, bytes_transferred);
  if (error) {
    return;
  }
  if (bytes_transferred > 0) {
    std::cout << "SocketSessionForReader::bytes:" << bytes_transferred << std::endl; // For debug
    bytesReceived_ = bytes_transferred;
    sendAck();
    bytesReceived_ = 0;
  }
}
} // namespace gramsballoon::pgrams