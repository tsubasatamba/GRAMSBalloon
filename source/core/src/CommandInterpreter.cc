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

void CommandInterpreter::parse()
{
  code_ = 0;
  arguments_.clear();

  const int header_length = 2;
  const int footer_length = 2 + MD5_DIGEST_LENGTH;
  if (command_.size() <= header_length + footer_length) {
    std::cerr << "Coud not parse command: command length = " << command_.size() << std::endl;
    return;
  }
  int index = header_length;
  code_ = command_[index];
  index++;

  while (index<static_cast<int>(command_.size())-footer_length) {
    arguments_.push_back(command_[index]);
    index++;
  }
}

void CommandInterpreter::interpret()
{
  
}

