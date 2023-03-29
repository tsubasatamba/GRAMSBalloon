#ifndef CommandDefinition_H
#define CommandDefinition_H 1


#include <iostream>
#include <vector>
#include <sys/time.h>
#include "CRC16.hh"

/**
 * A class to define telemetry
 *
 * @author Tsubasa Tamba, Shota Arai
 * @date 2023-03-25
 */

namespace gramsballoon {

class CommandDefinition
{
public:
  CommandDefinition();

  bool interpret();
  template<typename T> T getValue(int index);
  template<typename T> void getVector(int index, int num, std::vector<T>& vec);
  
  void setCommand(const std::vector<uint8_t>& v) { command_ = v; }

  const std::vector<uint8_t>& Command() const { return command_; }
  uint16_t Code() { return code_; }
  const std::vector<int32_t>& Arguments() const {return arguments_; }

  
private:
  std::vector<uint8_t> command_;
  uint16_t code_ = 0;
  uint16_t argc_ = 0;
  std::vector<int32_t> arguments_;

  // access to other classes
  
};

} /* namespace gramsballoon */

#endif /* CommandDefinition_H */