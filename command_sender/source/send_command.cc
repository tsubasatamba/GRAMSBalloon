#include <iostream>
#include <string>
#include "CommandSender.hh"
#include "CommandDefinition.hh"

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

  gramsballoon::CommandDefinition command_definition;
  std::vector<uint8_t> command
    = command_definition.make_byte_array(name, arg_array);

  gramsballoon::CommandSender sender;
  sender.set_serial_port("/dev/tty.usbserial-1130");
  if ( !sender.open_serial_port() ) {
    std::cout << "Serial port open error -> exit" << std::endl;
    return -1;
  }
  
  const int length_sent = sender.send(command);
  std::cout << "Length sent: " << length_sent << std::endl;
  
  sender.close_serial_port();

  return 0;
}
