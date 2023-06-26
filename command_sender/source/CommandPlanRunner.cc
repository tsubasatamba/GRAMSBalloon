/**
 * Command sending app for GRAMS Balloon at Taiki
 *
 * @author Hirokazu Odaka, Shota Arai
 *
 * @date 2023-03-30 | Hirokazu Odaka | prototyping
 */

#include <vector>
#include <string>
#include <sstream>
#include <istream>
#include <fstream>
#include <iomanip>
#include <anlnext/CLIUtility.hh>
#include "CommandSender.hh"
#include "CommandBuilder.hh"
#include "CommandSaver.hh"

using namespace gramsballoon;

void print_command(const std::vector<std::string>& commands, int run_index);
std::vector<std::vector<std::string>> read_command_plan(const std::string &filename);
void run_command_sequence(const std::vector<std::vector<std::string>> &commands);

void send_command(const std::vector<std::vector<std::string>> &commands, int run_index);

const int DISPLAY_NUMBER_PREVIOUS = 10;
const int DISPLAY_NUMBER_ADVANCE = 30;

int main(int argc, char **argv)
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

void print_command(const std::vector<std::vector<std::string>>& commands, int run_index)
{
  int disp_num_prev = std::min(DISPLAY_NUMBER_PREVIOUS, run_index);
  int disp_num_adv = std::min(static_cast<int>(commands.size()) - run_index, DISPLAY_NUMBER_ADVANCE);
  std::cout << "\n\n#############################################"
            << "\nlines command" << std::endl;
  for (int i = 0; i < DISPLAY_NUMBER_PREVIOUS - disp_num_prev; i++) {
    std::cout << "\n";
  }
  for (int i = disp_num_prev; i > 0; i--) {
    std::cout << std::setfill('0') << std::right << std::setw(3) << run_index - i << std::setfill(' ') << "   ";
    if (commands[run_index - i][0][0] == '#') {
      std::cout << "\x1b[40m";
    }
    for (int j = 0; j < commands[run_index - i].size(); j++) {
      std::cout << commands[run_index - i][j] << " ";
    }
    if (commands[run_index - i][0][0] == '#') {
      std::cout << "\x1b[49m";
    }
    std::cout << std::endl;
  }

  std::cout << std::setfill('0') << std::right << std::setw(3) << run_index << std::setfill(' ') << "   ";
  std::cout << "\x1b[42m";
  for (int i = 0; i < commands[run_index].size(); i++) {
    std::cout << commands[run_index][i] << " ";
  }
  std::cout << "\x1b[49m"
            << " <=" << std::endl;

  for (int i = 1; i < disp_num_adv; i++) {
    std::cout << std::setfill('0') << std::right << std::setw(3) << run_index + i << std::setfill(' ') << "   ";
    for (int j = 0; j < commands[run_index + i].size(); j++) {
      std::cout << commands[run_index + i][j] << " ";
    }
    std::cout << std::endl;
  }

  if (DISPLAY_NUMBER_ADVANCE > disp_num_adv) {
    std::cout << "\x1b[41m"
              << "EOF"
              << "\x1b[49m";
    for (int i = 0; i < DISPLAY_NUMBER_ADVANCE - disp_num_adv; i++) {
      std::cout << "\n";
    }
  }

  std::cout << "#############################################\n\n"
            << std::endl;
}

std::vector<std::vector<std::string>> read_command_plan(const std::string &filename)
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
      continue;
    }
    else if ((com_args[0][0] != '#')) {
      try {
        builder.get_command_property(com_args[0]);
      }
      catch (CommandException &e) {
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
  anlnext::ReadLine reader;
  int run_index = 0;
  const std::vector<std::string> completion_candidate = {"send", "back", "skip", "exit", "goto"};
  reader.set_completion_candidates(completion_candidate);
  while (true) {
    if (run_index >= static_cast<int>(commands.size())) {
      break;
    }
    if (commands[run_index].size() == 0) {
      run_index++;
      continue;
    }
    if (commands[run_index][0][0] == '#') {
      run_index++;
      continue;
    }
    print_command(commands, run_index);
    const int count = reader.read("INPUT> ");
    std::cout << std::endl;
    const std::string line = reader.str();
    if (line == "send") {
      send_command(commands, run_index);
      run_index++;
      continue;
    }
    else if (line == "exit") {
      std::cout << "Exiting the command sender." << std::endl;
      break;
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
      continue;
    }
    else if (line == "goto") {
      std::cout << "Which line do you want to go?" << std::endl;
      reader.read("INPUT> ");
      const std::string destination_str = reader.str();
      const int destination = std::stoi(destination_str);
      if (destination >= commands.size() || destination < 0) {
        std::cout << "Invalid value" << std::endl;
        continue;
      }
      if (commands[destination].size() == 0) {
        std::cout << "This line is blank" << std::endl;
        continue;
      }
      else if (commands[destination][0][0] == '#') {
        std::cout << "This line is comment" << std::endl;
        continue;
      }
      else {
        std::cout << "Go to line " << destination << std::endl;
        run_index = destination;
        continue;
      }
    }
    else if (line == "skip") {
      std::cout << "Skipped line " << run_index << std::endl;
      run_index++;
      continue;
    }
    else {
      std::cout << "Error: invalid input." << std::endl;
      continue;
    }
  }
}

void send_command(const std::vector<std::vector<std::string>> &commands, int run_index)
{
  CommandBuilder builder;
  std::vector<int> args;
  for (int i = 1; i < static_cast<int>(commands[run_index].size()); i++) {
    args.push_back(std::stoi(commands[run_index][i]));
  }
  std::vector<uint8_t> command_bits = builder.make_byte_array(commands[run_index][0], args);
  CommandSender sender;
  sender.set_serial_port("/dev/tty.usbserial-14410");
  //sender.set_serial_port("/dev/ttyAMA0");
  if (!sender.open_serial_port()) {
    std::cout << "Serial port open error -> Skip" << std::endl;
    return;
  }
  int rval = sender.send(command_bits);
  if (rval != command_bits.size()) {
    std::cout << "Send Error" << std::endl;
  }
  else {
    std::cout << "Command " << commands[run_index][0] << " sent." << std::endl;
    write_command(command_bits, commands[run_index][0]);
  }
  sender.close_serial_port();
}
