#ifndef CommandDefinition_H
#define CommandDefinition_H 1


#include <iostream>
#include <vector>
#include <sys/time.h>
#include <openssl/md5.h>

/**
 * A class to define telemetry
 *
 * @author Tsubasa Tamba, Shota Arai
 * @date 2023-03-25
 */

namespace GRAMSBalloon {

class CommandDefinition
{
public:
  CommandDefinition();

  bool isValid();
  void parse();
  void interpret();
  
  void setCommand(const std::vector<uint8_t>& v) { command_ = v; }

  const std::vector<uint8_t>& Command() const { return command_; }
  uint16_t Code() { return code_; }
  const std::vector<uint8_t>& Arguments() const {return arguments_; }

  
private:
  std::vector<uint8_t> command_;
  uint16_t code_ = 0;
  std::vector<uint8_t> arguments_;

  // access to other classes
  
};

} /* namespace GRAMSBalloon */

#endif /* CommandDefinition_H */
