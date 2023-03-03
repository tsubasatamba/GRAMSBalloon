/**
 * Receive commands from ground.
 *
 * @author Tsubasa Tamba, Shota Arai
 * @date 2023-03-01
 */


#ifndef ReceiveCommand_H
#define ReceiveCommand_H 1

#include <anlnext/BasicModule.hh>
#include "SerialCommunication.hh"

class ReceiveCommand : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(ReceiveCommand, 1.0);

public:
  ReceiveCommand();
  virtual ~ReceiveCommand();
  
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_finalize() override;

private:
  std::unique_ptr<SerialCommunication> sc_ = nullptr;
  speed_t inputBaudrate_;
  speed_t outputBaudrate_;
  std::string serialPath_;
  char openMode_;
  std::vector<uint8_t> buffer_;
  int length_ = 1;
};

#endif /* ReceiveCommand_H */
