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

class SimpleLoop : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(SimpleLoop, 1.0);
public:
  SimpleLoop();
  virtual ~SimpleLoop();
  
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_pre_initialize() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_begin_run() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_end_run() override;
  anlnext::ANLStatus mod_finalize() override;

private:
};

#endif /* SimpleLoop_H */
