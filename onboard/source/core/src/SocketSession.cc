#include "SocketSession.hh"
namespace gramsballoon::pgrams {
void SocketSession::on_send(const boost::system::error_code &error, std::size_t bytes_transferred) {
  if (!error){
    std::cout << "Sent " << bytes_transferred << " bytes." << std::endl;
  }
}
void SocketSession::on_receive(const boost::system::error_code &error, std::size_t bytes_transferred) {
  if (!error) {
    std::cout << "Received " << bytes_transferred << " bytes from " << socket_.remote_endpoint().address() << ":" << socket_.remote_endpoint().port() << std::endl;
  }
}
} // namespace gramsballoon::pgrams