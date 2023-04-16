#ifndef GetSystemStatus_HH
#define GetSystemStatus_HH 1

#include <anlnext/BasicModule.hh>
#include "SystemControl.hh"

namespace gramsballoon{

/**
 * @brief Sample Module for system control
 * @author Shota Arai
 * @date 2023-04-16
 */

class GetSystemStatus:public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(GetSystemStatus, 1.0);
  ENABLE_PARALLEL_RUN();

public:
  GetSystemStatus();
  virtual ~GetSystemStatus();

protected:
  GetSystemStatus(const GetSystemStatus &r) = default;

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_analyze() override;
  int FreeSize() { return freeSize_; }
  int AllSize() { return allSize_; }

private:
  int freeSize_;
  int allSize_;
  std::shared_ptr<SystemStatus> systemStatus_;
};



GetSystemStatus::~GetSystemStatus()
{
}

}/*namespace gramsballoon*/

#endif /*GetSystemStatus_HH*/