#include "ReceiveTelemetry.hh"
using namespace anlnext;

namespace gramsballoon {

ReceiveTelemetry::ReceiveTelemetry()
  : maxTelemetry_(32000), baudrate_(B57600), openMode_(O_RDWR)
{
  serialPath_ = "/dev/null";
}

ReceiveTelemetry::~ReceiveTelemetry() = default;

ANLStatus ReceiveTelemetry::mod_define()
{
  define_parameter("baudrate", &mod_class::baudrate_);
  define_parameter("serial_path", &mod_class::serialPath_);
  define_parameter("open_mode", &mod_class::openMode_);
  define_parameter("chatter", &mod_class::chatter_);
  
  return AS_OK;
}

ANLStatus ReceiveTelemetry::mod_initialize()
{
  buffer_.resize(maxTelemetry_);
  sc_ = std::make_unique<SerialCommunication>(serialPath_, baudrate_, openMode_);
  sc_->initialize();
  
  return AS_OK;
}

ANLStatus ReceiveTelemetry::mod_analyze()
{  
  fd_set fdset;
  timeval timeout;
  timeout.tv_sec = 10;
  timeout.tv_usec = 0;
  FD_ZERO(&fdset);
  FD_SET(sc_->FD(), &fdset);
  int rv = select(sc_->FD() + 1, &fdset, NULL, NULL, &timeout);
  if (rv == -1) {
    std::cerr << "Error in ReceiveTelemetry::mod_analyze: rv = -1" << std::endl;
    return AS_ERROR;
  }

  if (rv == 0) {
    std::cout << "Time out" << std::endl;
    return AS_OK;
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  const int status = sc_->sread(buffer_, maxTelemetry_);
  if (status == -1) {
    std::cerr << "Read command failed in ReceiveTelemetry::mod_analyze: status = " << status << std::endl;
    return AS_OK;
  }
  
  for (int i = 0; i < status; i++) {
    valid_ = false;
    if (i<status-1 && buffer_[i]==0xeb && buffer_[i+1]==0x90) {
      telemetry_.clear();
    }
    telemetry_.push_back(buffer_[i]);
    if (i>0 && buffer_[i-1]==0xc5 && buffer_[i]==0xc5) {
      valid_ = true;
      break;
    }
  }
  if (chatter_>=1) {
    std::cout << "status: " << status << std::endl;
    for (int i = 0; i < static_cast<int>(telemetry_.size());i++) {
      std::cout << "telemetry[" <<i<< "] = "<<static_cast<int>(telemetry_[i]) << std::endl;
    }
  }

  return AS_OK;
}

ANLStatus ReceiveTelemetry::mod_finalize()
{
  return AS_OK;
}

} // namespace gramsballoon
