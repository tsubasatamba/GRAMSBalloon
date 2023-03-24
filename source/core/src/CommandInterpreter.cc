#include "CommandInterpreter.hh"
#include <thread>
#include <chrono>


CommandInterpreter::CommandInterpreter()
{
}

bool CommandInterpreter::isValid()
{
  const int n = command_.size();
  if (n<4) {
    std::cerr << "Command is too short!!: length = " << n << std::endl; 
    return false;
  }
  std::vector<unsigned char> md(MD5_DIGEST_LENGTH);
  MD5(&command_[0], n-2-MD5_DIGEST_LENGTH, &md[0]);

  const int start_index = n-2-MD5_DIGEST_LENGTH;
  for (int i=0; i<MD5_DIGEST_LENGTH; i++) {
    if (command_[start_index+i] != static_cast<uint8_t>(md[i])) {
      return false;
    }
  }
  return true;
}

