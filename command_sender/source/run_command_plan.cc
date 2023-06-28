/**
 * Command sending app for GRAMS Balloon at Taiki
 *
 * @author Hirokazu Odaka, Shota Arai
 *
 * @date 2023-03-30 | Hirokazu Odaka | prototyping
 */

#include <vector>
#include <string>
#include <istream>
#include <fstream>
#include <sstream>
#include <anlnext/CLIUtility.hh>
#include "CommandSender.hh"
#include "CommandBuilder.hh"
#include "CommandSaver.hh"

using namespace gramsballoon;

void print_command(const std::vector<std::string>& commands, int run_index, bool all_lines=false);
std::vector<std::vector<std::string>> read_command_plan(const std::string& filename);
void run_command_sequence(const std::vector<std::vector<std::string>>& commands);
void send_command(const std::vector<std::string>& command);
void print_help();

constexpr int DISPLAY_NUMBER_BEFORE = 8;
constexpr int DISPLAY_NUMBER_AFTER  = 12;


int main(int argc, char** argv)
{
  if (argc != 2) {
    std::cout << "Usage: CommandPlanRunner <command-plan>" << std::endl;
    return 1;
  }

  const std::string filename(argv[1]);
  const std::vector<std::vector<std::string>> commands = read_command_plan(filename);
  run_command_sequence(commands);

  return 0;
}

void print_command(const std::vector<std::vector<std::string>>& commands, int run_index, bool all_lines=false)
{
  std::cout << "\n\n"
            << "#############################################\n";

  for (int line_index=0; line_index<commands.size(); line_index++) {
    if (all_lines || (run_index-DISPLAY_NUMBER_BEFORE <= line_index && line_index <= run_index+DISPLAY_NUMBER_AFTER)) {
      std::cout << std::setfill('0') << std::right << std::setw(3) << line_index << std::setfill(' ') << "   ";
      if (line_index==run_index) { std::cout << "\x1b[42m"; }
      for (const auto& s: commands[line_index]) {
        std::cout << s << " ";
      }
      if (line_index==run_index) { std::cout << "\x1b[49m"; }
      std::cout << std::endl;
    }
  }

  std::cout << "#############################################\n"
            << std::endl;
}

std::vector<std::vector<std::string>> read_command_plan(const std::string& filename)
{
  CommandBuilder builder;
  std::vector<std::vector<std::string>> commands;
  std::unique_ptr<std::ifstream> ifs = std::make_unique<std::ifstream>(filename, std::ios_base::in);
  std::string line;
  while (std::getline(*ifs, line)) {
    std::string temp;
    std::stringstream linestream{line};
    std::vector<std::string> com_args;
    while (std::getline(linestream, temp, ' ')) {
      com_args.push_back(temp);
    }
    if (com_args.size() == 0) {
      com_args.push_back("#####");
      //continue;
    }
    else if ((com_args[0][0] != '#')) {
      try {
        builder.get_command_property(com_args[0]);
      }
      catch (CommandException& e) {
        std::cout << "Command exception caught: " << e.print() << " in " << com_args[0] << " <- Exit" << std::endl;
        exit(1);
      }
      if (static_cast<int>(com_args.size()) - 1 != builder.get_argnum(com_args[0])) {
        std::cout << "Invalid args in " << com_args[0] << " <- Exit" << std::endl;
        exit(1);
      }
    }
    commands.push_back(com_args);
  }
  ifs->close();
  return commands;
}

void run_command_sequence(const std::vector<std::vector<std::string>>& commands)
{
  print_command(commands, -1, true);

  anlnext::ReadLine reader;
  const std::vector<std::string> completion_candidate = {"send", "back", "skip", "exit", "goto", "help"};
  reader.set_completion_candidates(completion_candidate);

  int run_index = 0;
  bool print_flag = true;

  while (true) {
    if (run_index >= static_cast<int>(commands.size())) {
      break;
    }
    if (commands[run_index].size() == 0 || commands[run_index][0][0] == '#') {
      run_index++;
      continue;
    }

    if (print_flag) {
      print_command(commands, run_index);
      print_flag = false;
    }

    const int count = reader.read("INPUT> ");
    const std::string line = reader.str();
    if (line == "") {
      ;
    }
    else if (line == "help") {
      print_help();
    }
    else if (line == "send") {
      send_command(commands[run_index]);
      run_index++;
      print_flag = true;
    }
    else if (line == "skip") {
      std::cout << "Skipped line " << run_index << std::endl;
      run_index++;
      print_flag = true;
    }
    else if (line == "back") {
      int i = run_index;
      while (1) {
        if (i <= 0) {
          std::cout << "Can't back anymore" << std::endl;
          break;
        }
        i--;
        if (commands[i].size() == 0) {
          continue;
        }
        if (commands[i][0][0] == '#') {
          continue;
        }
        run_index = i;
        break;
      }
      print_flag = true;
    }
    else if (line == "goto") {
      std::cout << "Which line do you want to go?" << std::endl;
      reader.read("INPUT> ");
      const std::string destination_str = reader.str();
      const int destination = std::stoi(destination_str);
      if (destination >= commands.size() || destination < 0) {
        std::cout << "Invalid value" << std::endl;
      }
      else if (commands[destination].size() == 0) {
        std::cout << "This line is blank" << std::endl;
      }
      else if (commands[destination][0][0] == '#') {
        std::cout << "This line is comment" << std::endl;
      }
      else {
        std::cout << "Go to line " << destination << std::endl;
        run_index = destination;
        print_flag = true;
      }
    }
    else if (line == "exit") {
      std::cout << "Exiting the command sender." << std::endl;
      break;
    }
    else {
      std::cout << "Error: invalid input." << std::endl;
    }
  }
}

void send_command(const std::vector<std::string>& command)
{
  const std::string command_name = command[0];
  std::vector<int> arg_array;
  for (std::size_t i=1; i<command.size(); i++) {
    arg_array.push_back(std::stoi(command[i]));
  }

  CommandBuilder builder;
  std::vector<uint8_t> command_byte_array = builder.make_byte_array(command_name, arg_array);

  CommandSender sender;
  sender.set_serial_port("/dev/tty.usbserial-14410");
  if ( !sender.open_serial_port() ) {
    std::cout << "Serial port open error -> Skip" << std::endl;
    return;
  }

  int rval = sender.send(command_byte_array);
  if (rval != command_byte_array.size()) {
    std::cout << "Send Error" << std::endl;
  }
  else {
    std::cout << "Command " << command_name << " sent." << std::endl;
    write_command(command_byte_array, command_name);
  }

  sender.close_serial_port();
}

void print_help()
{
  std::cout << "\n"
            << "#### HELP ####\n"
            << "#\n"
            << "# send \n"
            << "# skip \n"
            << "# back \n"
            << "# goto <N> \n"
            << "# exit \n"
            << "# help \n"
            << "# \n"
            << "##############\n"
            << std::endl;
}
