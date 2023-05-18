#include "CommandDefinition.hh"
#include <thread>
#include <chrono>
#include <iomanip>
#include <fstream>

namespace gramsballoon {

CommandDefinition::CommandDefinition()
{
}

bool CommandDefinition::setCommand(const std::vector<uint8_t>& v)
{
  const int n = v.size();
  if (n<6) {
    std::cerr << "Command is too short!!: length = " << n << std::endl; 
    return false;
  }

  if (v[0]!=0xeb || v[1]!=0x90) {
    std::cerr << "start code incorect" << std::endl;
    return false;
  }
  if (v[n-2]!=0xc5 || v[n-1]!=0xc5) {
    std::cerr << "stop code incorrect" << std::endl;
    return false;
  }

  command_ = v;
  uint16_t argc = getValue<uint16_t>(4);

  if (n != 10 + 4 * static_cast<int>(argc)) {
    std::cerr << "Invalid command: length not appropriate" << std::endl;
    std::cerr << "The length of command should be " << 10+4*static_cast<int>(argc) <<
      ", but now it is " << n << std::endl;
    return false;
  }

  std::vector<uint8_t> com_without_fotter;
  for (int i=0; i<n-4; i++) {
    com_without_fotter.push_back(v[i]);
  }
  uint16_t crc_calc = calcCRC16(com_without_fotter);
  uint16_t crc_attached = getValue<uint16_t>(n-4);
  if (crc_calc != crc_attached) {
    std::cerr << "Invalid command: CRC16 not appropriate" << std::endl;
    return false;
  }
  
  return true;
}

void CommandDefinition::interpret()
{
  code_ = getValue<uint16_t>(2);
  argc_ = getValue<uint16_t>(4);

  arguments_.clear();
  getVector<int32_t>(6, static_cast<int>(argc_), arguments_);
}

void CommandDefinition::writeFile(const std::string& filename, bool append)
{
  std::ofstream ofs;
  if (append) {
    ofs = std::ofstream(filename, std::ios::app|std::ios::binary);
  }
  else {
    ofs = std::ofstream(filename, std::ios::out|std::ios::binary);
  }
  if (!ofs) {
    std::cerr << "File open error." << std::endl;
    return;
  }
  const int size = command_.size();
  ofs.write(reinterpret_cast<char*>(&command_[0]), size);
  ofs.flush();
  ofs.close();
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
  if (byte > 8) {
    std::cerr << "CommandDefinition::getValue error: typename error" << std::endl;
    std::cerr << "byte should be equal to or less than 8: byte = " << byte << std::endl;
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
  if (byte > 8) {
    std::cerr << "CommandDefinition::getVector error: typename error" << std::endl;
    std::cerr << "byte should be equal to or less than 8: byte = " << byte << std::endl;
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
