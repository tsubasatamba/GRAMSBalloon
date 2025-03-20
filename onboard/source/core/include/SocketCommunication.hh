#ifndef GB_SocketCommunication_hh
#define GB_SocketCommunication_hh 1
#include "sys/socket.h"
#include <arpa/inet.h>
#include <string>

namespace gramsballoon::pgrams {
class SocketCommunication {
public:
  SocketCommunication() = default;
  virtual ~SocketCommunication() = default;

protected:
  SocketCommunication(const SocketCommunication &r) = default;

private:
  int socket_ = 0;
  sockaddr_in serverAddress;
  int port = 0;
  std::string ip = "";
};
} // namespace gramsballoon::pgrams
#endif //GB_SocketCommunication_hh