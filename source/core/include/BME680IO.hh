#ifndef BME680IO_H
#define BME680IO_H 1

#include <pigpiod_if2.h>
#include <iostream>
#include <chrono>
#include <thread>
#include "bme68x.h"
#include "SPIInterface.hh"

#define BME_REGISTER_BYTES 1
#define SPI_CS_ENABLE 0

#if SPI_CS_ENABLE == 0
#define CS_ENABLE PI_OFF
#define CS_DISABLE PI_ON
#elif SPI_CS_ENABLE == 1
#define CS_ENABLE PI_ON
#define CS_DISABLE PI_OFF
#endif

/**
 * A class to handle BME680
 *
 * @author Tsubasa Tamba, Shota Arai
 * @date 2023-03-01
 */
class BME680IO
{
public:
  BME680IO();
  static int8_t readReg(uint8_t reg_addr, uint8_t* reg_data, uint32_t length, void* intf_ptr);
  static int8_t writeReg(uint8_t reg_addr, const uint8_t *reg_data, uint32_t length, void *intf_ptr);
  static void delay(uint32_t period, void *intf_ptr);
  void setup(SPIInterface* intf);
  int getData();
  void printData();
  
private:
  std::unique_ptr<bme68x_dev> bme68xn_ = nullptr;
  std::unique_ptr<bme68x_data> sensorData_ = nullptr;
  std::unique_ptr<bme68x_conf> configure_ = nullptr;
};



#endif /* BME680IO_H */
