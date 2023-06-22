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
#include <anlnext/CLIUtility.hh>
#include "CommandSender.hh"
#include "CommandBuilder.hh"
#include "CommandSaver.hh"

using namespace gramsballoon;

void print_command(const std::vector<std::string> commands, int run_index);
std::vector<std::vector<std::string>> read_command_plan(const std::string &filename);
void run_command_sequence(const std::vector<std::vector<std::string>> &commands);

const int DISPLAY_NUMBER_PREVIOUS = 2;
const int DISPLAY_NUMBER_ADVANCE = 2;

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cout << "Usage: grams_command_sender <command-plan>" << std::endl;
        return 1;
    }
    const std::string filename(argv[1]);
    const std::vector<std::vector<std::string>> commands = read_command_plan(filename);
    run_command_sequence(commands);

    return 0;
}

void print_command(const std::vector<std::vector<std::string>> commands, int run_index)
{
    std::cout << run_index << std::endl;
    int disp_num_prev = std::min(DISPLAY_NUMBER_PREVIOUS, run_index);
    int disp_num_adv = std::min(static_cast<int>(commands.size()) - run_index - 1, DISPLAY_NUMBER_ADVANCE);
    std::cout << "disp_num_adv: " << disp_num_adv << " disp_num_prev:" << disp_num_prev << std::endl;
    for (int i = disp_num_prev; i > 0; i--)
    {
        // std::cout << "i: " << i << std::endl;
        for (int j = 0; j < commands[run_index - i].size(); j++)
        {
            std::cout << commands[run_index - i][j] << " ";
        }
        std::cout << std::endl;
    }

    std::cout << "\x1b[42m";
    for (int i = 0; i < commands[run_index].size(); i++)
    {
        std::cout << commands[run_index][i] << " ";
    }
    std::cout << "\x1b[49m"
              << " <=" << std::endl;

    for (int i = 0; i < disp_num_adv; i++)
    {
        // std::cout << "i: " << i << std::endl;
        for (int j = 0; j < commands[run_index + i].size(); j++)
        {
            std::cout << commands[run_index + i][j] << " ";
        }
        std::cout << std::endl;
    }
}

std::vector<std::vector<std::string>> read_command_plan(const std::string &filename)
{
    CommandBuilder builder;
    std::vector<std::vector<std::string>> commands;
    std::unique_ptr<std::ifstream> ifs = std::make_unique<std::ifstream>(filename, std::ios_base::in);
    std::string line;
    while (std::getline(*ifs, line))
    {
        std::string temp;
        std::stringstream linestream{line};
        std::vector<std::string> com_args;
        while (std::getline(linestream, temp, ' '))
        {
            com_args.push_back(temp);
        }

        try
        {
            builder.get_command_property(com_args[0]);
        }
        catch (CommandException &e)
        {
            std::cout << "Command exception caught: " << e.print() << " in " << com_args[0] << std::endl;
            exit(1);
        }

        if (com_args.size() - 1 != builder.get_argnum(com_args[0]))
        {
            std::cout << "Invalid args in " << com_args[0] << std::endl;
            exit(1);
        }
        commands.push_back(com_args);
    }
    ifs->close();
    return commands;
}

void run_command_sequence(const std::vector<std::vector<std::string>> &commands)
{
    anlnext::ReadLine reader;
    while (true)
    {
        int run_index = 0;
        print_command(commands, run_index);
        const int count = reader.read("COM> ");
        const std::string line = reader.str();
        std::cout << line << std::endl;
        if (line == "send")
        {
            CommandBuilder builder;
            std::vector<int> args;
            for (int i = 1; i < commands[run_index].size(); i++)
            {
                args.push_back(atoi(commands[run_index][i].c_str()));
            }
            std::vector<uint8_t> command_bits = builder.make_byte_array(commands[run_index][0], args);
            CommandSender sender;
            sender.set_serial_port("/dev/tty.usbserial-14410");
            if (!sender.open_serial_port())
            {
                std::cout << "Serial port open error -> exit" << std::endl;
                return;
            }
            sender.send(command_bits);
            std::cout << "Command sent." << std::endl;
            sender.close_serial_port();
        }
        else if (line == "exit")
        {
            std::cout << "Exiting the command sender." << std::endl;
            break;
        }
        else
        {
            std::cout << "Error: invalid input." << std::endl;
        }
    }
}