#ifndef ErrorManager_H
#define ErrorManager_H 1


#include <iostream>
#include <vector>
#include <string>
#include "magic_enum.hpp"

/**
 * A class to handle software error
 *
 * @author Tsubasa Tamba,
 * @date 2023-04-27
 */

namespace gramsballoon {

enum class ErrorType {
  RECEIVE_COMMAND_SERIAL_COMMUNICATION_ERROR = 0,
  RECEIVE_COMMAND_SELECT_ERROR = 1,
  RECEIVE_COMMAND_SREAD_ERROR = 2,
  INVALID_COMMAND = 3,
  PIGPIO_START_ERROR = 4,
  SPI_OPEN_ERROR = 5,
  RTD_DATA_AQUISITION_ERROR_1 = 6,
  RTD_DATA_AQUISITION_ERROR_2 = 7,
  RTD_DATA_AQUISITION_ERROR_3 = 8,
  RTD_DATA_AQUISITION_ERROR_4 = 9,
  RTD_DATA_AQUISITION_ERROR_5 = 10,
  ENV_DATA_AQUISITION_ERROR_1 = 11,
  ENV_DATA_AQUISITION_ERROR_2 = 12,
  ENV_DATA_AQUISITION_ERROR_3 = 13,
  ENV_DATA_AQUISITION_ERROR_4 = 14,
  ENV_DATA_AQUISITION_ERROR_5 = 15,
  ACCEL_DEVICE_NOT_FOUND = 16,
  ACCEL_DATA_AQUISITION_ERROR = 17,
  SLOW_ADC_DATA_AQUISITION_ERROR = 18,
  GET_SD_CAPACITY_ERROR = 19,
  ANALOG_DISCOVERY_INITIALIZE_ERROR = 20,
  ANALOG_DISCOVERY_NOT_CONNECTED = 21,
  ONLY_ONE_ANALOG_DISCOVERY_CONNECTED = 22,
  TPC_HV_INVALID_CHANNEL = 23,
  TPC_HV_INVALID_VOLTAGE = 24,
  PMT_HV_INVALID_CHANNEL = 25,
  PMT_HV_INVALID_VOLTAGE = 26,
  TRIGGER_SETUP_ERROR = 27,
  TOO_FEW_EVENTS_DETECTED = 28,
  SEND_TELEMETRY_SERIAL_COMMUNICATION_ERROR = 29,
  SEND_TELEMETRY_SWRITE_ERROR = 30,
  SHUTDOWN_REJECTED = 31,
  REBOOT_REJECTED = 32,
  MODULE_ACCESS_ERROR = 62,
  OTHER_ERRORS = 63
};

class ErrorManager
{
public:
  ErrorManager();
  void resetError();
  void setError(ErrorType v);
  static int strToBit(const std::string& s);
  static std::string bitToStr(int v);
  
  uint64_t ErrorCode() { return errorCode_; }

private:
  uint64_t errorCode_ = 0;
};

} /* namespace gramsballoon */

#endif /* ErrorManager_H */
