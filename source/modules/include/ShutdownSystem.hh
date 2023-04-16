#ifndef ShutdownSystem_HH
#define ShutdownSystem_HH 1

/**
 * Test module for shutdown system
 * @author Shota Arai
 * @date 2023-04-16
 */

#include <anlnext/BasicModule.hh>
#include "SystemControl.hh"

namespace gramsballoon
{
  class ShutdownSystem : public anlnext::BasicModule
  {
    DEFINE_ANL_MODULE(ShutdownSystem, 1.0);
    ENABLE_PARALLEL_RUN();

  public:
    ShutdownSystem() = default;
    virtual ~ShutdownSystem() = default;

  protected:
    ShutdownSystem(const ShutdownSystem &r) = default;

  public:
    anlnext::ANLStatus mod_define() override;
    anlnext::ANLStatus mod_initialize() override;
    anlnext::ANLStatus mod_finalize() override;

  private:
    bool confirmed_ = false;
    bool reboot_ = true;
  };

} /*namespace gramsballoon*/

#endif /*ShutdownSystem_HH*/