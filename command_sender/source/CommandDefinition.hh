#ifndef GRAMSBALOON_COMMANDDEFINITION_H
#define GRAMSBALOON_COMMANDDEFINITION_H 1

#include <cstdint>
#include <vector>
#include <map>
#include <string>

namespace gramsballoon
{

struct CommandException
{
};

struct CommandProperty
{
  uint16_t code = 0;
  int argnum = 0;
};

class CommandDefinition
{
public:
  CommandDefinition();
  ~CommandDefinition() = default;

  CommandProperty get_command_property(const std::string& name) const;
  uint16_t get_command_code(const std::string& name) const;
  int get_argnum(const std::string& name) const;

  std::vector<uint8_t> make_byte_array(const std::string& name, const std::vector<int32_t>& arg_array) const;

private:
  std::map<std::string, CommandProperty> code_map_;
};

} /* namespace gramsballoon */

#endif /* GRAMSBALOON_COMMANDDEFINITION_H */
