#ifndef CommandInterpreter_H
#define CommandInterpreter_H 1


#include <iostream>
#include <vector>
#include <sys/time.h>
#include <openssl/md5.h>
#include "BME680IO.hh"
#include "MAX31865IO.hh"
#include "DAQIO.hh"



/**
 * A class to define telemetry
 *
 * @author Tsubasa Tamba, Shota Arai
 * @date 2023-03-25
 */
class CommandInterpreter
{
public:
  CommandInterpreter();

  bool isValid();
  void parse();
  void interpret();
  
  void setCommand(std::vector<uint8_t>& v) { command_ = v; }
  void setDAQIO(DAQIO* io) { daqio_ = io; }

  
private:
  std::vector<uint8_t> command_;
  uint8_t code_;
  std::vector<uint8_t> arguments_;

  // access to other classes
  DAQIO* daqio_ = nullptr;
};



#endif /* CommandInterpreter_H */
