#include <iostream>
#include <iomanip>
#include <string>
#include "CommandBuilder.hh"

int main(int argc, char *argv[])
{
  if (argc < 2) {
    std::cout << "Usage: send_command <command name> <arg list>" << std::endl;
    return 0;
  }

  const std::string name(argv[1]);
  std::vector<int32_t> arg_array;
  for (int i=2; i<argc; i++) {
    arg_array.push_back(std::stoi(argv[i]));
  }

  std::vector<uint8_t> command;
  gramsballoon::CommandBuilder command_builder;
  try {
    command = command_builder.make_byte_array(name, arg_array);
  }
  catch (gramsballoon::CommandException& e) {
    std::cout << "Command exception caught: " << e.print() << std::endl;
    return 1;
  }

  std::cout << std::hex << std::uppercase;
  for (const uint8_t c: command) {
    std::cout << static_cast<int>(c) << " "; 
  }
  std::cout << std::endl;
  std::cout << std::dec << std::nouppercase;

  return 0;
}
