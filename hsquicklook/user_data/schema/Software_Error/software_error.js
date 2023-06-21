var Va = 5.026 //[V] for slow ADC
var Rref = 430 //[ohm] for RTD measurement
var Rshunt = 100 //[ohm] for Pressure gauge

HSQuickLook.main.schema =
    [
      {
	      "collection": "grams",
	      "directory": "Telemetry",
	      "document": "HK",
	      "period": 1,
	      "section": "Header",
	      "contents": {
          "Start_Code": {"type": "int", "format": "0x%04X"},
          "Telemetry_Type": {"type": "int"},
          "Time": {"type": "int"},
          "Time_us": {"type": "int"},
          "Telemetry_Index": {"type": "int"},
          "Run_ID": {"type": "int"}  
        }
      },
      {
	      "collection": "grams",
	      "directory": "Telemetry",
	      "document": "HK",
	      "period": 1,
	      "section": "Software_Error",
	      "contents": {
          "RECEIVE_COMMAND_SERIAL_COMMUNICATION_ERROR": {"type": "string"},
          "RECEIVE_COMMAND_SELECT_ERROR": {"type": "string"},
          "RECEIVE_COMMAND_SREAD_ERROR": {"type": "string"},
          "INVALID_COMMAND": {"type": "string"},
          "PIGPIO_START_ERROR": {"type": "string"},
          "SPI_OPEN_ERROR": {"type": "string"},
          "RTD_DATA_AQUISITION_ERROR_1": {"type": "string"},
          "RTD_DATA_AQUISITION_ERROR_2": {"type": "string"},
          "RTD_DATA_AQUISITION_ERROR_3": {"type": "string"},
          "RTD_DATA_AQUISITION_ERROR_4": {"type": "string"},
          "RTD_DATA_AQUISITION_ERROR_5": {"type": "string"},
          "ENV_DATA_AQUISITION_ERROR_1": {"type": "string"},
          "ENV_DATA_AQUISITION_ERROR_2": {"type": "string"},
          "ENV_DATA_AQUISITION_ERROR_3": {"type": "string"},
          "ENV_DATA_AQUISITION_ERROR_4": {"type": "string"},
          "ENV_DATA_AQUISITION_ERROR_5": {"type": "string"},
          "ACCEL_DEVICE_NOT_FOUND": {"type": "string"},
          "ACCEL_DATA_AQUISITION_ERROR": {"type": "string"},
          "SLOW_ADC_DATA_AQUISITION_ERROR": {"type": "string"},
          "GET_SD_CAPACITY_ERROR": {"type": "string"},
          "ANALOG_DISCOVERY_INITIALIZE_ERROR": {"type": "string"},
          "ANALOG_DISCOVERY_NOT_CONNECTED": {"type": "string"},
          "ONLY_ONE_ANALOG_DISCOVERY_CONNECTED": {"type": "string"},
          "TPC_HV_INVALID_CHANNEL": {"type": "string"},
          "TPC_HV_INVALID_VOLTAGE": {"type": "string"},
          "PMT_HV_INVALID_CHANNEL": {"type": "string"},
          "PMT_HV_INVALID_VOLTAGE": {"type": "string"},
          "TRIGGER_SETUP_ERROR": {"type": "string"},
          "SEND_TELEMETRY_SERIAL_COMMUNICATION_ERROR": {"type": "string"},
          "SEND_TELEMETRY_SWRITE_ERROR": {"type": "string"},
          "SHUTDOWN_REJECTED": {"type": "string"},
          "REBOOT_REJECTED": {"type": "string"},
          "MODULE_ACCESS_ERROR": {"type": "string"},
          "OTHER_ERRORS": {"type": "string"}
        }
      }
    ];


