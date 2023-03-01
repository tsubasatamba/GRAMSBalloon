#ifndef SPIInterface_H
#define SPIInterface_H 1



/**
 * A class of SPI Interface
 *
 * @author Tsubasa Tamba, Shota Arai
 * @date 2023-03-01
 */
class SPIInterface
{
public:
  void setSPIHandler(unsigned int v) { SPIHandler_ = v; }
  void setChipSelect(int v) { chipSelect_ = v; }
  void setGPIOHandler(int v) { GPIOHandler_ = v; }
  unsigned int SPIHandler() { return SPIHandler_; }
  int ChipSelect() { return chipSelect_; }
  int GPIOHandler() { return GPIOHandler_; }

private:
  unsigned int SPIHandler_ = 0;
  int chipSelect_ = -1;
  int GPIOHandler_ = -1;
};



#endif /* SPIInterface_H */
