#ifndef GB_ReceiveStatusFromDAQComputer_hh
#define GB_ReceiveStatusFromDAQComputer_hh 1
#include "anlnext/BasicModule.hh"

namespace gramsballoon::pgrams {
class ReceiveStatusFromDAQComputer: public anlnext::BasicModule {
  DEFINE_ANL_MODULE(ReceiveStatusFromDAQComputer, 1.0);
  ENABLE_PARALLEL_RUN();

public:
  ReceiveStatusFromDAQComputer() = default;
  virtual ~ReceiveStatusFromDAQComputer() = default;

protected:
  ReceiveStatusFromDAQComputer(const ReceiveStatusFromDAQComputer &r) = default;

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_pre_initialize() override;
  anlnext::ANLStatus mod_begin_run() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_end_run() override;
  anlnext::ANLStatus mod_finalize() override;

private:
};
} // namespace gramsballoon::pgrams
#endif // GB_ReceiveStatusFromDAQComputer_hh