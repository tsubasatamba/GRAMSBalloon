/**
 * SimpleLoop sample module of the ANL Next framework
 *
 * @author 
 * @date
 *
 */

#ifndef SimpleLoop_H
#define SimpleLoop_H 1

#include <anlnext/BasicModule.hh>
#include <cstdint>

namespace gramsballoon {

class SimpleLoop : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(SimpleLoop, 1.0);
  ENABLE_PARALLEL_RUN();
public:
  SimpleLoop();
  virtual ~SimpleLoop();
protected:
  SimpleLoop(const SimpleLoop& r) = default;

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_pre_initialize() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_begin_run() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_end_run() override;
  anlnext::ANLStatus mod_finalize() override;

private:
  int int_v = 0;
  double double_v = 0.0;
  std::string string_v;
  std::vector<int> v1;
  std::vector<double> v2;
  std::vector<std::string> v3;
};

} /* namespace gramsballoon */

#endif /* SimpleLoop_H */
