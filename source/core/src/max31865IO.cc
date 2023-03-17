#include "MAX31865IO.hh"
#include <vector>
#include <iomanip>

MAX31865IO::MAX31865IO()
{
}

void MAX31865IO::setInterface(SPIInterface* intf)
{
  intf_ = intf;
}

int MAX31865IO::readReg(int8_t reg, char *value, unsigned int length)
{
  const int pi = intf_->GPIOHandler();
  const int cs = intf_->ChipSelect();
  const unsigned int spihandler = intf_->SPIHandler();
  gpio_write(pi, cs, PI_OFF);
  reg &= ~MAX31865_WRITE_MSK;
  int rslt = MAX31865_OK;
  const int rslt_write = spi_write(pi, spihandler, reinterpret_cast<char *>(&reg), 1);
  if (rslt_write != 1) {
    if (rslt_write<0) {
      std::cerr << "read_reg failed in writing address (error code: " << pigpio_error(rslt_write) << ")" << std::endl;
    }
    else {
      std::cerr << "read_reg failed in writing data: rslt_write = " << rslt_write << std::endl;
    }
    rslt = rslt_write;
    return rslt;
  }
  
  const int rslt_read = spi_read(pi, spihandler, value, length);

  if (rslt_read != static_cast<int>(length)) {
    if (rslt_read<0) {
      std::cerr << "read_reg failed in reading data (error code: " << pigpio_error(rslt_read) << ")" << std::endl;
    }
    else {
      std::cerr << "read_reg failed in writing data: rslt_read = " << rslt_read << std::endl;
    }
    rslt = rslt_read;
    return rslt;
  }

  gpio_write(pi, cs, PI_ON);
  rslt = rslt_read;
  return rslt;
}

int MAX31865IO::writeReg(uint8_t reg, char *value, unsigned int length)
{
  const int pi = intf_->GPIOHandler();
  const int cs = intf_->ChipSelect();
  const unsigned int spihandler = intf_->SPIHandler();

  gpio_write(pi, cs, PI_OFF);
  reg |= MAX31865_WRITE_MSK;
  int rslt = MAX31865_OK;
  int rslt_write = spi_write(pi, spihandler, reinterpret_cast<char *>(&reg), 1);
  if (rslt_write != 1) {
    if (rslt_write<0) {
      std::cerr << "write_reg failed in writing address (error code: " << pigpio_error(rslt_write) << ")" << std::endl;
    }
    else {
      std::cerr << "write_reg failed in writing data: rslt_write = " << rslt_write << std::endl;
    }
    rslt = rslt_write;
    return rslt;
  }
  
  rslt_write = spi_write(pi, spihandler, value, length);
  if (rslt_write != static_cast<int>(length)) {
    if (rslt_write < 0) {
      std::cerr << "write_reg failed in writing data (error code: " << pigpio_error(rslt_write) << ")" << std::endl;
    }
    else {
      std::cerr << "write_reg failed in writing data: rslt_write = " << rslt_write << std::endl;
    }
    rslt = rslt_write;
    return rslt;
  }

  gpio_write(pi, cs, PI_ON);
  rslt = rslt_write;
  return rslt;
}

char MAX31865IO::configureBits()
{
  char bits = 0;
  
  bits |= (bias_ & MAX31865_CONF_BIAS_MSK);
  bits |= (conversion_ & MAX31865_CONF_CONVERSION_AUTO);
  bits |= (faultDetection_ & (MAX31865_CONF_FD_CYCLE_MSK | MAX31865_CONF_F_STATUS_CLR_MSK));
  bits |= (wires_ & MAX31865_CONF_3WIRE_MSK);
  bits |= (filter_ & MAX31865_CONF_FILTER_SEL_MSK);
  return bits;
}

int MAX31865IO::setConfigure()
{
  char bits = configureBits();
  int _bias = bias_;

  if (getConfigure() < 0) {
    std::cerr << "set_configure failed due to get_configure failure." << std::endl;
    return -1;
  }
  if (conversion_ == MAX31865_CONF_CONVERSION_AUTO) {
    bits = (bits & (~MAX31865_CONF_FILTER_SEL_MSK)) | (filter & MAX31865_CONF_FILTER_SEL_MSK);
  }

  const int pi = intf_->GPIOHandler();
  const int cs = intf_->ChipSelect();
  const unsigned int spihandler = intf_->SPIHandler();

  int dbg = write_reg(pi, spihandler, cs, MAX31865_CONF_WRITE_REG, &bits, 1);
  if (dbg != 1) {
    std::cerr << "set_configure failed due to write_reg failure. write_reg_bytes: " << dbg << std::endl;
  }
  if ((_bias != bias) && (_bias == MAX31865_CONF_BIAS_ON)) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // should be fixed
  }
  
  return MAX31865_OK;
}

int MAX31865IO::getConfigure()
{
  const int pi = intf_->GPIOHandler();
  const int cs = intf_->ChipSelect();
  const unsigned int spihandler = intf_->SPIHandler();
  
  char bits;
  int rslt = read_reg(pi, spihandler, cs, MAX31865_CONF_READ_REG, &bits, 1);
  if (rslt != 1) {
    std::cerr << "get_configure failed to read. read bytes: " << rslt << std::endl;
    return MAX31865_BAD;
  }
  bias_ = bits & MAX31865_CONF_BIAS_MSK;
  conversion_ = bits & MAX31865_CONF_CONVERSION_MSK;
  wires_ = bits & MAX31865_CONF_3WIRE_MSK;
  faultDetection_ = bits & (MAX31865_CONF_FD_CYCLE_MSK | MAX31865_CONF_F_STATUS_CLR_MSK);
  filter_ = bits & MAX31865_CONF_FILTER_SEL_MSK;

  return MAX31865_OK;
}

int16_t MAX31865IO::getData()
{
  const int pi = intf_->GPIOHandler();
  const int cs = intf_->ChipSelect();
  const unsigned int spihandler = intf_->SPIHandler();
  
  int16_t data = 0;
  char temp[2];
  read_reg(pi, spihandler, cs, MAX31865_DATA_REG_MSB, &temp[0], 1);
  read_reg(pi, spihandler, cs, MAX31865_DATA_REG_LSB, &temp[1], 1);

  if ((temp[1] & MAX31865_DATA_FAULT_MSK) == 0) {
    data |= temp[0] << 7;
    data |= temp[1] >> 1;
  }
  else {
    data = 0;
    std::cerr << "MAX31865IO::getData() deteted fault." << std::endl;
    getFaultStatus();
    faultStatusClear();
    return MAX31865_BAD;
  }
  return data;
}

int MAX31865IO::faultStatusClear()
{
  if (get_configure() < 0) {
    std::cerr << "fault_status_clear may change configure due to get_configure failure." << std::endl;
    return MAX31865_WARNING;
  }
  char bits = configureBits();
  bits &= ~MAX31865_CONF_1SHOT_MSK;
  bits &= ~MAX31865_CONF_FD_CYCLE_MSK;
  bits |= MAX31865_CONF_F_STATUS_CLR_MSK;

  int rslt_write = write_reg(pi, handle, cs, MAX31865_CONF_WRITE_REG, &bits, 1);
  std::this_thread::sleep_for(std::chrono::microseconds(100));
  if (rslt_write != 1) {
    std::cerr << "fault_status_clear may not be executed. rslt_write = " << rslt_write << std::endl;
    return MAX31865_BAD;
  }
  return MAX31865_OK;
}

int MAX31865IO::getFaultStatus()
{
  std::cout << "get_fault_status called" << std::endl;
  char bits = 0b0;
  int rslt_read = read_reg(pi, handle, cs, MAX31865_FAULT_STATUS_REG, &bits, 1);
  if (rslt_read != 1) {
    std::cerr << "get_fault_status failed due to read_reg failure. rslt_read = " << rslt_read << std::endl;
  }
  std::cout << "Fault status : " << std::bitset<8>(bits) << std::endl;
  return MAX31865_OK;
}

double MAX31865IO::Tconversion(int adc)
{
  const int r_ref = 430;
  const double ret = static_cast<double>(adc) * static_cast<double>(r_ref) / 400.0 * 32.0 - 256.0;

  return ret;
}



