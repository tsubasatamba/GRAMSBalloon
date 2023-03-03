#include "ReceiveCommand.hh"
#include <chrono>
#include <thread>

using namespace anlnext;

ReceiveCommand::ReceiveCommand()
  :inputBaudrate_(B1200), outputBaudrate_(B57600), openMode_(O_RDWR)
{
  sc_ = std::make_unique<SerialCommunication>();
  serialPath_ = "/dev/null";
}

ReceiveCommand::~ReceiveCommand() = default;

ANLStatus ReceiveCommand::mod_define()
{
  define_parameter("input_baudrate", &mod_class::inputBaudrate_);
  define_parameter("output_baudrate", &mod_class::outputBaudrate_);
  define_parameter("serial_path", &mod_class::serialPath_);
  define_parameter("open_mode", &mod_class::openMode_);
  define_parameter("length", &mod_class::length_);

  return AS_OK;
}

ANLStatus ReceiveCommand::mod_initialize()
{
  sc_->setBaudrate(inputBaudrate_, outputBaudrate_);
  sc_->setSerialPath(serialPath_);
  sc_->setOpenMode(openMode_);

  sc_->initialize();

  return AS_OK;
}

ANLStatus ReceiveCommand::mod_analyze()
{
  sc_->sread(buffer_, length_);
  std::cout << buffer_.size() << std::endl;
  if (buffer_.size()>0) {
    std::cout << buffer_[0] << std::endl;
  }

  //std::this_thread::sleep_for(std::chrono::seconds(1));
  
  return AS_OK;
}

ANLStatus ReceiveCommand::mod_finalize()
{
  return AS_OK;
}
