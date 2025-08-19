#ifndef GB_CommunicationFormat_H
#define GB_CommunicationFormat_H 1

#include "CRC16.hh"
#include <cstdint>
#include <iostream>
#include <sys/time.h>
#include <vector>

/**
 * A class to define the command format for the pGRAMS system.
 *
 * @author Tsubasa Tamba, Shota Arai
 * @date 2023-03-25
 * @date 2025-03-11 | Modified to fit the pGRAMS command format.
 * @date 2025-04-01 | Name changed from CommandDefinition to CommunicationFormat
 */

namespace gramsballoon::pgrams {

class CommunicationFormat {
public:
  CommunicationFormat();

  bool setData(const std::vector<uint8_t> &v);
  void interpret();
  void writeFile(const std::string &filename, bool append);
  template <typename T>
  T getValue(int index);
  template <typename T>
  void getVector(int index, int num, std::vector<T> &vec);

  const std::vector<uint8_t> &Command() const { return command_; }
  std::string CommandStr() const { return std::string(command_.begin(), command_.end()); }
  uint16_t Code() const { return code_; }
  uint16_t Argc() const { return argc_; }
  const std::vector<int32_t> &Arguments() const { return arguments_; }
  void update();
  void setCode(uint16_t code) { code_ = code; }
  void setArgc(uint16_t argc) {
    argc_ = argc;
    arguments_.resize(argc_);
  }
  void setArguments(const std::vector<int32_t> &arguments) {
    setArgc(static_cast<uint16_t>(arguments.size()));
    for (uint16_t i = 0; i < argc_; ++i) {
      arguments_[i] = arguments[i];
    }
  }
  void setArguments(uint16_t index, int32_t argument) {
    arguments_[index] = argument;
  }
  int32_t getArguments(uint16_t index) const {
    if (index < argc_) {
      return arguments_[index];
    }
    return 0;
  }
  void setCommand(const std::vector<uint8_t> &command) { command_ = command; }

private:
  std::vector<uint8_t> command_;
  uint16_t code_ = 0;
  uint16_t argc_ = 0;
  std::vector<int32_t> arguments_;
};

} // namespace gramsballoon::pgrams

#endif /* CommandDefinition_H */
