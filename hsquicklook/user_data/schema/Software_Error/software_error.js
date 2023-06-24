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
          "RECEIVE_COMMAND_SERIAL_COMMUNICATION_ERROR": {"type": "string", "status": function(v){return (v=="Error")?"error": "safe"; }},
          "RECEIVE_COMMAND_SELECT_ERROR": {"type": "string", "status": function(v){return (v=="Error")?"error": "safe"; }},
          "RECEIVE_COMMAND_SREAD_ERROR": {"type": "string", "status": function(v){return (v=="Error")?"error": "safe"; }},
          "INVALID_COMMAND": {"type": "string", "status": function(v){return (v=="Error")?"error": "safe"; }},
          "PIGPIO_START_ERROR": {"type": "string", "status": function(v){return (v=="Error")?"error": "safe"; }},
          "SPI_OPEN_ERROR": {"type": "string", "status": function(v){return (v=="Error")?"error": "safe"; }},
          "RTD_DATA_AQUISITION_ERROR_1": {"type": "string", "status": function(v){return (v=="Error")?"error": "safe"; }},
          "RTD_DATA_AQUISITION_ERROR_2": {"type": "string", "status": function(v){return (v=="Error")?"error": "safe"; }},
          "RTD_DATA_AQUISITION_ERROR_3": {"type": "string", "status": function(v){return (v=="Error")?"error": "safe"; }},
          "RTD_DATA_AQUISITION_ERROR_4": {"type": "string", "status": function(v){return (v=="Error")?"error": "safe"; }},
          "RTD_DATA_AQUISITION_ERROR_5": {"type": "string", "status": function(v){return (v=="Error")?"error": "safe"; }},
          "ENV_DATA_AQUISITION_ERROR_1": {"type": "string", "status": function(v){return (v=="Error")?"error": "safe"; }},
          "ENV_DATA_AQUISITION_ERROR_2": {"type": "string", "status": function(v){return (v=="Error")?"error": "safe"; }},
          "ENV_DATA_AQUISITION_ERROR_3": {"type": "string", "status": function(v){return (v=="Error")?"error": "safe"; }},
          "ENV_DATA_AQUISITION_ERROR_4": {"type": "string", "status": function(v){return (v=="Error")?"error": "safe"; }},
          "ENV_DATA_AQUISITION_ERROR_5": {"type": "string", "status": function(v){return (v=="Error")?"error": "safe"; }},
          "ACCEL_DEVICE_NOT_FOUND": {"type": "string", "status": function(v){return (v=="Error")?"error": "safe"; }},
          "ACCEL_DATA_AQUISITION_ERROR": {"type": "string", "status": function(v){return (v=="Error")?"error": "safe"; }},
          "SLOW_ADC_DATA_AQUISITION_ERROR": {"type": "string", "status": function(v){return (v=="Error")?"error": "safe"; }},
          "GET_SD_CAPACITY_ERROR": {"type": "string", "status": function(v){return (v=="Error")?"error": "safe"; }},
          "ANALOG_DISCOVERY_INITIALIZE_ERROR": {"type": "string", "status": function(v){return (v=="Error")?"error": "safe"; }},
          "ANALOG_DISCOVERY_NOT_CONNECTED": {"type": "string", "status": function(v){return (v=="Error")?"error": "safe"; }},
          "ONLY_ONE_ANALOG_DISCOVERY_CONNECTED": {"type": "string", "status": function(v){return (v=="Error")?"error": "safe"; }},
          "TPC_HV_INVALID_CHANNEL": {"type": "string", "status": function(v){return (v=="Error")?"error": "safe"; }},
          "TPC_HV_INVALID_VOLTAGE": {"type": "string", "status": function(v){return (v=="Error")?"error": "safe"; }},
          "PMT_HV_INVALID_CHANNEL": {"type": "string", "status": function(v){return (v=="Error")?"error": "safe"; }},
          "PMT_HV_INVALID_VOLTAGE": {"type": "string", "status": function(v){return (v=="Error")?"error": "safe"; }},
          "TRIGGER_SETUP_ERROR": {"type": "string", "status": function(v){return (v=="Error")?"error": "safe"; }},
          "SEND_TELEMETRY_SERIAL_COMMUNICATION_ERROR": {"type": "string", "status": function(v){return (v=="Error")?"error": "safe"; }},
          "SEND_TELEMETRY_SWRITE_ERROR": {"type": "string", "status": function(v){return (v=="Error")?"error": "safe"; }},
          "SHUTDOWN_REJECTED": {"type": "string", "status": function(v){return (v=="Error")?"error": "safe"; }},
          "REBOOT_REJECTED": {"type": "string", "status": function(v){return (v=="Error")?"error": "safe"; }},
          "MODULE_ACCESS_ERROR": {"type": "string", "status": function(v){return (v=="Error")?"error": "safe"; }},
          "OTHER_ERRORS": {"type": "string", "status": function(v){return (v=="Error")?"error": "safe"; }}
      }}
    ];


