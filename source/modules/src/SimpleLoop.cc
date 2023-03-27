#include "SimpleLoop.hh"
//#include "AstroUnits.hh"

using namespace anlnext;

namespace GRAMSBalloon {

SimpleLoop::SimpleLoop()
{
}

SimpleLoop::~SimpleLoop() = default;

ANLStatus SimpleLoop::mod_define()
{
  define_parameter("int_v", &mod_class::int_v);
  //define_parameter("double_v", &mod_class::double_v, 1.0, "s");
  define_parameter("string_v", &mod_class::string_v);
  define_parameter("v1", &mod_class::v1);
  //define_parameter("v2", &mod_class::v2);
  define_parameter("v3", &mod_class::v3);
  
  std::cout << "int_v: " << int_v << std::endl;
  std::cout << "double_v: " << double_v << std::endl;
  return AS_OK;
}

ANLStatus SimpleLoop::mod_pre_initialize()
{
  return AS_OK;
}

ANLStatus SimpleLoop::mod_initialize()
{
  return AS_OK;
}

ANLStatus SimpleLoop::mod_begin_run()
{
  return AS_OK;
}

ANLStatus SimpleLoop::mod_analyze()
{
  return AS_OK;
}

ANLStatus SimpleLoop::mod_end_run()
{
  return AS_OK;
}

ANLStatus SimpleLoop::mod_finalize()
{
  return AS_OK;
}

} /* namespace GRAMSBalloon */