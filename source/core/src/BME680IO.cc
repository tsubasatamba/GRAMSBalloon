#include "BME680IO.hh"
#include <vector>
#include <iomanip>

namespace GRAMSBalloon {

BME680IO::BME680IO()
{
  bme68xn_ = std::make_unique<bme68x_dev>();
  sensorData_ = std::make_unique<bme68x_data>();
  configure_ = std::make_unique<bme68x_conf>();
}

int8_t BME680IO::readReg(uint8_t reg_addr, uint8_t* reg_data, uint32_t length, void *intf_ptr)
{
  int8_t rslt = BME68X_OK;
  SPIInterface* intf = static_cast<SPIInterface *>(intf_ptr);
  const int pi = intf->GPIOHandler();
  const int cs = intf->ChipSelect();
  const unsigned int spihandler = intf->SPIHandler();
  
  set_mode(pi, cs, PI_OUTPUT);
  gpio_write(pi, cs, CS_ENABLE);
  #if 0
  int dbg = gpio_read(pi, cs);
  std::cout << "gpio_read: " << dbg << std::endl;
  #endif
  char* address = reinterpret_cast<char *>(&reg_addr);

  int write_status = spi_write(pi, spihandler, address, BME_REGISTER_BYTES);
  if (write_status != BME_REGISTER_BYTES) {
    if (write_status==0) {
      write_status = -1000000;
    }
    std::cerr << "spi_write error: write_status = " << write_status << ", BME_REGISTER_BYTES = " << BME_REGISTER_BYTES << std::endl;
    rslt = static_cast<int8_t>(write_status);
    gpio_write(pi, cs, CS_DISABLE);
    return rslt;
  }

  std::vector<char> temp(length, 0);
  
  int read_status = spi_read(pi, spihandler, &temp[0], length);
  if (read_status != static_cast<int>(length)) {
    if (read_status==0) {
      read_status = -1000000;
    }
    std::cerr << "spi_read error: read_status = " << read_status << ", length = " << length << std::endl;
    rslt = static_cast<int8_t>(read_status);
    gpio_write(pi, cs, CS_DISABLE);
    return rslt;
  }
    
  for (int i = 0; i < static_cast<int>(length); i++) {
    reg_data[i] = temp[i];   
  }
  
  #if 0
  std::cout << "reg_addr: " << std::hex << static_cast<int>(0x7F &  reg_addr) << std::endl;
  for (int i=0; i<static_cast<int>(length); i++) {
    std::cout << static_cast<int>(temp[i]) << " ";
  }
  for (int i=0; i<static_cast<int>(length); i++) {
    std::cout << static_cast<int>(reg_data[i]) << " ";
  }
  std::cout << std::dec << std::endl;
  #endif

  gpio_write(pi, cs, CS_DISABLE);
  #if 0
  int dbg2 = gpio_read(pi, cs);
  std::cout << "gpio_read: " << dbg2 << std::endl;
  #endif
    
  return rslt;
}

int8_t BME680IO::writeReg(uint8_t reg_addr, const uint8_t *reg_data, uint32_t length, void *intf_ptr)
{
  int8_t rslt = BME68X_OK;
  SPIInterface* intf = static_cast<struct SPIInterface*>(intf_ptr);
  const int pi = intf->GPIOHandler();
  const int cs = intf->ChipSelect();
  const unsigned int spihandler = intf->SPIHandler();
  
  gpio_write(pi, cs, CS_ENABLE);
  int write_status = spi_write(pi, spihandler, reinterpret_cast<char *>(&reg_addr), BME_REGISTER_BYTES);
  if (write_status != BME_REGISTER_BYTES) {
    if (write_status==0) {
      write_status = -1000000;
    }
    std::cerr << "spi_write error: write_status = " << write_status << ", BME_REGISTER_BYTES = " << BME_REGISTER_BYTES << std::endl;
    rslt = static_cast<int8_t>(write_status);
    gpio_write(pi, cs, CS_DISABLE);
    return rslt;
  }
  
  write_status = spi_write(pi, spihandler, reinterpret_cast<char *>(const_cast<uint8_t *>(reg_data)), length);
  if (write_status != static_cast<int>(length)) {
    if (write_status==0) {
      write_status = -1000000;
    }
    std::cerr << "spi_write error: write_status = " << write_status << ", length = " << length << std::endl;
    rslt = static_cast<int8_t>(write_status);
    gpio_write(pi, cs, CS_DISABLE);
    return rslt;
  }
  
  gpio_write(pi, cs, CS_DISABLE);
  
  return rslt;
}

void BME680IO::delay(uint32_t period, void *intf_ptr)
{
  std::this_thread::sleep_for(std::chrono::microseconds(period));
}

void BME680IO::setup(SPIInterface* intf)
{
  configure_->os_hum = BME68X_OS_1X;
  configure_->os_pres = BME68X_OS_1X;
  configure_->os_temp = BME68X_OS_1X;
  configure_->filter = BME68X_FILTER_OFF;
  
  bme68xn_->intf = bme68x_intf::BME68X_SPI_INTF;
  bme68xn_->read = BME680IO::readReg;
  bme68xn_->write = BME680IO::writeReg;
  bme68xn_->delay_us = BME680IO::delay;
  bme68xn_->intf_ptr = intf;
  
  sensorData_->pressure = 0;
  sensorData_->humidity = 0;
  sensorData_->temperature = 0;
  
  bme68x_init(bme68xn_.get());
  bme68x_set_conf(configure_.get(), bme68xn_.get());
}


int BME680IO::getData()
{
  bme68x_init(bme68xn_.get());
  bme68x_set_conf(configure_.get(), bme68xn_.get());
  bme68x_set_op_mode(BME68X_FORCED_MODE, bme68xn_.get());
  uint8_t ndata = 0;
  int res = bme68x_get_data(BME68X_FORCED_MODE, sensorData_.get(), &ndata, bme68xn_.get());
 

  if (res!=0) {
    std::cerr << "BME680IO::getData failed: ndata = " << ndata << ", get_data_id = " << res << std::endl;
  }
  bme68x_set_op_mode(BME68X_SLEEP_MODE, bme68xn_.get());
  

  return res;
}

void BME680IO::printData()
{
  std::cout << "Pressure: " << sensorData_->pressure << "\nHumidity: " << sensorData_->humidity << "\nTemperature: " << sensorData_->temperature << std::endl;
}

} /* namespace GRAMSBalloon */




