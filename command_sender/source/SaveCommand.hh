#ifndef SaveCommand_HH
#define SaveCommand_HH 1

#include <string>
#include <fstream>
#include <iomanip>
#include <vector>

namespace gramsballoon
{

void write_command(const std::vector<uint8_t>& command, const std::string& log_filename);
std::string get_time_stamp_str();

} /*namespace gramsballoon*/

#endif
