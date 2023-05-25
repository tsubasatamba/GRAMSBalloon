#include "CommandDefinition.hh"
#include <tuple>

namespace
{
uint16_t crc_calc(std::vector<uint8_t> byte_array)
{
  uint16_t crc = 0;
  for (const uint8_t i: byte_array) {
    crc = crc ^ i;
    for (int j=0; j<8; j++) {
      if (crc & 1) {
        crc = (crc>>1)^0x8408;
      }
      else {
        crc >>= 1;
      }
    }
  }
  return crc;
}

} /* anonymous namespace */

namespace gramsballoon
{

CommandDefinition::CommandDefinition()
{
  code_map_["Get_Status"]             = CommandProperty{100, 0};
  code_map_["Reset_Error"]            = CommandProperty{101, 0};
  code_map_["Exec_Shutdown"]          = CommandProperty{102, 0};
  code_map_["Exec_Reboot"]            = CommandProperty{103, 0};
  code_map_["Prepare_Shutdown"]       = CommandProperty{104, 0};
  code_map_["Prepare_Reboot"]         = CommandProperty{105, 0};
  code_map_["Start_Detector_Readout"] = CommandProperty{201, 0};
  code_map_["Stop_Detector_Readout"]  = CommandProperty{202, 0};
  code_map_["Set_Trigger_Mode"]       = CommandProperty{203, 1};
  code_map_["Set_Trigger_Channel"]    = CommandProperty{204, 2}; 
  code_map_["Set_ADC_Offset"]         = CommandProperty{205, 3};
  code_map_["Exec_TPC_HV_Output"]     = CommandProperty{206, 0};
  code_map_["Set_TPC_HV "]            = CommandProperty{207, 1}; 
  code_map_["Exec_PMT_HV_Output"]     = CommandProperty{208, 0};
  code_map_["Set_PMT_HV"]             = CommandProperty{209, 1};
  code_map_["Get_Waveform"]           = CommandProperty{210, 0};
  code_map_["Set_Trigger_Level"]      = CommandProperty{211, 1};
  code_map_["Set_Trigger_Position"]   = CommandProperty{212, 1};
  code_map_["Dummy_1"]                = CommandProperty{900, 0}; 
  code_map_["Dummy_2"]                = CommandProperty{901, 1};
  code_map_["Dummy_3"]                = CommandProperty{902, 0};
}

CommandProperty CommandDefinition::get_command_property(const std::string& name) const
{
  auto command = code_map_.find(name);
  if (command == code_map_.end()) {
    throw CommandException();
  }
  
  return command->second;
}

uint16_t CommandDefinition::get_command_code(const std::string& name) const
{
  return get_command_property(name).code;
}

int CommandDefinition::get_argnum(const std::string& name) const
{
  return get_command_property(name).argnum;
}

std::vector<uint8_t> CommandDefinition::make_byte_array(const std::string& name, const std::vector<int32_t>& arg_array) const
{
  std::vector<uint8_t> command;
  command.push_back(0xEB);
  command.push_back(0x90);

  const CommandProperty property = get_command_property(name);
  const int code = property.code;
  const int argnum = property.argnum;
  command.push_back((code & 0xFF00u) >> 8);
  command.push_back((code & 0x00FFu) >> 0);
  command.push_back((argnum & 0xFF00u) >> 8);
  command.push_back((argnum & 0x00FFu) >> 0);

  if (argnum != arg_array.size()) {
    throw CommandException();
  }

  for (const int32_t arg: arg_array) {
    command.push_back((arg & 0xFF000000u) >> 24);
    command.push_back((arg & 0x00FF0000u) >> 16);
    command.push_back((arg & 0x0000FF00u) >>  8);
    command.push_back((arg & 0x000000FFu) >>  0);
  }

  const uint16_t crc = crc_calc(command);
  command.push_back((crc & 0xFF00u) >> 8);
  command.push_back((crc & 0x00FFu) >> 0);

  // termination word C5C5
  command.push_back(0xC5);
  command.push_back(0xC5);
  
  return command;
}

} /* namespace gramsballoon */
