#include "SocketCommunication.hh"
#include <iostream>
#include <signal.h>
namespace gramsballoon::pgrams {
void SigPipeHander(int) {
  std::cout << "Caught SIGPIPE!" << std::endl;
}
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
    break;
  }
  if (failed_) {
    std::cerr << "Error in SocketCommunication: Connection failed, despite " << numTrial_ << " times trials." << std::endl;
    return -1;
  }
  return 0;
}
void SocketCommunication::HandleSIGPIPE() {
  struct sigaction sa;
  sa.sa_handler = SigPipeHander;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGPIPE, &sa, NULL);
}
} // namespace gramsballoon::pgrams