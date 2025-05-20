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
  uint16_t Code() const { return code_; }
  uint16_t Argc() const { return argc_; }
  const std::vector<int32_t> &Arguments() const { return arguments_; }
  void update();
  void setCode(uint16_t code) { code_ = code; }
  void setArgc(uint16_t argc) { argc_ = argc; }
  void setArguments(const std::vector<int32_t> &arguments) {
    argc_ = static_cast<uint16_t>(arguments.size());
    arguments_ = arguments;
  }

private:
  std::vector<uint8_t> command_;
  uint16_t code_ = 0;
  uint16_t argc_ = 0;
  std::vector<int32_t> arguments_;
};

} // namespace gramsballoon::pgrams

#endif /* CommandDefinition_H */
