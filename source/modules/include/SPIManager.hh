/**
 * SPI Manager
 *
 * @author Tsubasa Tamba, Shota Arai
 * @date 2023-03-01
 */


#ifndef SPIManager_H
#define SPIManager_H 1

#include <anlnext/BasicModule.hh>
#include "SPIInterface.hh"

namespace GRAMSBalloon {

class SPIManager : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(SPIManager, 1.0);

public:
  SPIManager();
  virtual ~SPIManager();
  
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_finalize() override;

  SPIInterface* Interface() { return interface_.get(); }
  void addChipSelect(int v);

private:
  std::unique_ptr<SPIInterface> interface_ = nullptr;
  int channel_ = 0;
  int baudrate_ = 100000;
  int spiFlags_ = (1<<5) + (1<<6);
  const std::string portNumber_ = "8888";
  std::vector<int> chipSelectArray_;
};

} /* namespace GRAMSBalloon */

#endif /* SPIManager_H */
