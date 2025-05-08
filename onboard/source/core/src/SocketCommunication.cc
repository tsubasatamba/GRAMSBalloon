#include "SocketCommunication.hh"
#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/socket.h>
#undef GB_SOCKET_COMMUNICATION_CLIENT
namespace gramsballoon::pgrams {
void SigPipeHander(int) {
  std::cout << "Caught SIGPIPE!" << std::endl;
}
#ifdef GB_SOCKET_COMMUNICATION_CLIENT
int SocketCommunication::connect() {
  socket_ = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_ < 0) {
    std::cerr << "Error in SocketCommunication::connect: Socket creation failed." << std::endl;
    return socket_;
  }
  for (int i = 0; i < numTrial_; i++) {
    if (::connect(socket_, (sockaddr *)&serverAddress_, sizeof(serverAddress_)) < 0) {
      //std::cerr << "Error in SocketCommunication: Connection failed." << std::endl;
      failed_ = true;
      continue;
    }
    failed_ = false;
    std::cout << "SocketCommunication: Connected to server." << std::endl;
    break;
  }
  if (failed_) {
    std::cerr << "Error in SocketCommunication: Connection failed, despite " << numTrial_ << " times trials." << std::endl;
    return -1;
  }
  return 0;
}
#endif // GB_SOCKET_COMMUNICATION_CLIENT
void SocketCommunication::HandleSIGPIPE() {
  struct sigaction sa;
  sa.sa_handler = SigPipeHander;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGPIPE, &sa, NULL);
}
int SocketCommunication::WaitForTimeOut(timeval timeout) {
  fd_set fdset;
  FD_ZERO(&fdset);
  FD_SET(socket_, &fdset);
  int rv = select(socket_ + 1, &fdset, NULL, NULL, &timeout);
  return rv;
}
#ifndef GB_SOCKET_COMMUNICATION_CLIENT
int SocketCommunication::connect() {
  if (!binded_) {
    socketServer_ = socket(AF_INET, SOCK_STREAM, 0);
    if (socketServer_ < 0) {
      std::cerr << "Error in SocketCommunication::connect: Socket creation failed." << std::endl;
      return socketServer_;
    }
    bind(socketServer_, (sockaddr *)&serverAddress_, sizeof(serverAddress_));
    binded_ = true;
    std::cout << "Binding to " << inet_ntoa(serverAddress_.sin_addr) << ":" << ntohs(serverAddress_.sin_port) << std::endl;
  }
  if (listen(socketServer_, 1) == -1) {
    std::cerr << "Error in SocketCommunication: Listening failed." << std::endl;
    ::close(socketServer_);
    return -1;
  }
  sockaddr_in clientAddress;
  socklen_t clientAddressSize = sizeof(clientAddress);
  std::cout << "Waiting for connection..." << std::endl;
  socket_ = accept(socketServer_, (sockaddr *)&clientAddress, &clientAddressSize);
  if (socket_ == -1) {
    std::cerr << "Error in SocketCommunication: Accepting connection failed." << std::endl;
  }
  else {
    std::cout << "Connected to " << inet_ntoa(clientAddress.sin_addr) << std::endl;
  }
  return socket_;
}
#endif // GB_SOCKET_COMMUNICATION_CLIENT
} // namespace gramsballoon::pgrams