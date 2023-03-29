#include "GetRaspiStatus.hh"

using namespace anlnext;

namespace gramsballoon {

GetRaspiStatus::GetRaspiStatus()
{
  ifsTemp_ = std::make_shared<std::ifstream>("/sys/class/thermal/thermal_zone0/temp");
}

GetRaspiStatus::~GetRaspiStatus() = default;

ANLStatus GetRaspiStatus::mod_define()
{
  return AS_OK;
}

ANLStatus GetRaspiStatus::mod_initialize()
{
  return AS_OK;
}

ANLStatus GetRaspiStatus::mod_analyze()
{
  (*ifsTemp_) >> temperatureADC_;
  
  return AS_OK;
}


ANLStatus GetRaspiStatus::mod_finalize()
{
  ifsTemp_->close();
  return AS_OK;
}

} /* namespace gramsballoon */