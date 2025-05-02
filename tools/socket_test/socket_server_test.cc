#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <signal.h>

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <thread>

void SigPipeHandler(int) {
  std::cout << "Caught SIGPIPE!" << std::endl;
}

int main(int argc, char *argv[]) {
  const int num_trial = 10;
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <IP address> <port>" << std::endl;
    return 1;
  }
  bool failed = false;
  struct sigaction sa;
  sa.sa_handler = SigPipeHandler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGPIPE, &sa, NULL);

  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock == -1) {
    std::cerr << "Error : Socket creation failed." << std::endl;
    return 1;
  }
  sockaddr_in serverAddress;
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons(std::atoi(argv[2]));
  serverAddress.sin_addr.s_addr = inet_addr(argv[1]);

  bind(sock, (sockaddr *)&serverAddress, sizeof(serverAddress));
  std::cout << "Binding to " << argv[1] << ":" << argv[2] << std::endl;
  if (listen(sock, 1) == -1) {
    std::cerr << "Error : Listening failed." << std::endl;
    close(sock);
    return 1;
  }
  sockaddr_in clientAddress;
  socklen_t clientAddressSize = sizeof(clientAddress);
  std::cout << "Waiting for connection..." << std::endl;
  int vl = 1;
  int sock_res;
  ioctl(sock, FIONBIO, &vl);
  for (int i = 0; i < 10; i++) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    sock_res = accept(sock, (sockaddr *)&clientAddress, &clientAddressSize);
    if (sock_res == -1) {
      std::cerr << "Error : Accepting connection failed." << std::endl;
    }
    else {
      std::cout << "Connected to " << argv[1] << std::endl;
      break;
    }
  }
  char buffer[1024];
  int rev_size = recv(sock, buffer, sizeof(buffer), 0);
  buffer[rev_size] = '\0';
  if (rev_size == -1) {
    std::cerr << "Error : Receiving data failed." << std::endl;
    close(sock);
    return 1;
  }
  else {
    std::cout << "Received: " << buffer << std::endl;
  }
  close(sock);
  return 0;
}