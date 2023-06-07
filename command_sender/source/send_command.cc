#include <iostream>
#include <string>
#include <stdlib.h>
#include <fstream>
#include "CommandSender.hh"
#include "CommandBuilder.hh"
#include "CommandSaver.hh"

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

  std::string port_name;
  std::string setting_filename = getenv("HOME");
  setting_filename += "/settings/serial/command_port.txt";
  std::ifstream ifs(setting_filename);
  ifs >> port_name;
  ifs.close();

  std::vector<uint8_t> command;
  gramsballoon::CommandBuilder command_builder;
  try {
    command = command_builder.make_byte_array(name, arg_array);
  }
  catch (gramsballoon::CommandException& e) {
    std::cout << "Command exception caught: " << e.print() << std::endl;
    return 1;
  }

  gramsballoon::CommandSender sender;
  sender.set_serial_port(port_name);
  if ( !sender.open_serial_port() ) {
    std::cout << "Serial port open error -> exit" << std::endl;
    return -1;
  }

  const int length_sent = sender.send(command);
  std::cout << "Length sent: " << length_sent << std::endl;

  gramsballoon::write_command(command, name);

  sender.close_serial_port();
  
  return 0;
}
