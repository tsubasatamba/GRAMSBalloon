#include "SimpleLoop.hh"

using namespace anlnext;

SimpleLoop::SimpleLoop()
{
}

SimpleLoop::~SimpleLoop() = default;

ANLStatus SimpleLoop::mod_define()
{
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
