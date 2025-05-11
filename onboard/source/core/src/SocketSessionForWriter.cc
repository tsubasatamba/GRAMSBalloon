#include "SocketSessionForWriter.hh"
#include <boost/asio.hpp>
namespace gramsballoon::pgrams {
void SocketSessionForWriter::start() {
  sendHandler();
}
}