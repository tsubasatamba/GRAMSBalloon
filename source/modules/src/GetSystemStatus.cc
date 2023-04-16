#include "GetSystemStatus.hh"
#include <iostream>
using namespace anlnext;
namespace gramsballoon
{
GetSystemStatus::GetSystemStatus()
{
  systemStatus_ = std::make_shared<SystemStatus>();
}
ANLStatus GetSystemStatus::mod_define()
{
  return AS_OK;
}
ANLStatus GetSystemStatus::mod_analyze(){
  freeSize_ = static_cast<int>((systemStatus_->get_freesize()) / 1024 / 1024);
  allSize_ = static_cast<int>((systemStatus_->get_allsize()) / 1024 / 1024);

  #if 1
  std::cout << "free size(MB): " << freeSize_<<std::endl;
  std::cout << "all size(MB): " << allSize_<<std::endl;
  #endif
  return AS_OK;
}
}