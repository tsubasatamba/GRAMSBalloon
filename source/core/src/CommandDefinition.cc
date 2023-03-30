#include "CommandDefinition.hh"
#include <thread>
#include <chrono>
#include <iomanip>

namespace gramsballoon {

CommandDefinition::CommandDefinition()
{
}

bool CommandDefinition::interpret()
{
  const int n = command_.size();
  if (n<6) {
    std::cerr << "Command is too short!!: length = " << n << std::endl; 
    return false;
  }

  std::cout << "--- command received start ---" << std::endl;
  for (int i=0; i<(int)command_.size(); i++) {
    std::cout << i << " " << std::hex << static_cast<int>(command_[i]) << std::dec << std::endl;
  }
  std::cout << "--- command received end ---" << std::endl;

  code_ = getValue<uint16_t>(2);
  argc_ = getValue<uint16_t>(4);

  if (n != 10 + 4 * static_cast<int>(argc_)) {
    std::cerr << "Invalid command: length not appropriate" << std::endl;
    std::cerr << "The length of command should be " << 10+4*static_cast<int>(argc_) <<
      ", but now it is " << n << std::endl;
    return false;
  }

  std::vector<uint8_t> com_without_fotter;
  for (int i=0; i<n-4; i++) {
    com_without_fotter.push_back(command_[i]);
  }
  uint16_t crc_calc = calcCRC16(com_without_fotter);
  uint16_t crc_attached = getValue<uint16_t>(n-4);
  if (crc_calc != crc_attached) {
    std::cerr << "Invalid command: CRC16 not appropriate" << std::endl;
    return false;
  }

  arguments_.clear();
  getVector<int32_t>(6, static_cast<int>(argc_), arguments_);

  return true;
}

template<typename T>
T CommandDefinition::getValue(int index)
{
  const int n = command_.size();
  const int byte = sizeof(T);
  if (index+byte>n) {
    std::cerr << "CommandDefinition::getValue error: out of range" << std::endl;
    std::cerr << "command_.size() = " << n << ", index = " << index << ", byte = " << byte << std::endl;
    return static_cast<T>(0);
  }
  if (byte > 4) {
    std::cerr << "CommandDefinition::getValue error: typename error" << std::endl;
    std::cerr << "byte should be equal to or less than 4: byte = " << byte << std::endl;
    return static_cast<T>(0);
  }

  uint32_t v = 0;
  for (int i=0; i<byte; i++) {
    const int shift = 8 * (byte-1-i);
    v |= (command_[index+i] << (shift));
  }
  return static_cast<T>(v);
}

template<typename T>
void CommandDefinition::getVector(int index, int num, std::vector<T>& vec)
{
  const int n = command_.size();
  const int byte = sizeof(T);
  if (index+byte*num > n) {
    std::cerr << "CommandDefinition::getVector error: out of range" << std::endl;
    std::cerr << "command_.size() = " << n << ", index = " << index << ", byte = " << byte
    << ", num = " << num << std::endl;
    return;
  }
  if (byte > 4) {
    std::cerr << "CommandDefinition::getVector error: typename error" << std::endl;
    std::cerr << "byte should be equal to or less than 4: byte = " << byte << std::endl;
    return;
  }
  vec.clear();
  for (int i=0; i<num; i++) {
    T v = getValue<T>(index);
    vec.push_back(v);
    index += byte;
  }
}



} /* namespace gramsballoon */
