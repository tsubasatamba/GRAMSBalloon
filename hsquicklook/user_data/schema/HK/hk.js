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
        "Start_Code": { "type": "int", "format": "0x%04X" },
        "Telemetry_Type": { "type": "int" },
        "Time": { "type": "int" },
        "Time_us": { "type": "int" },
        "Telemetry_Index": { "type": "int" },
        "Run_ID": { "type": "int" }
      }
    },
    {
      "collection": "grams",
      "directory": "Telemetry",
      "document": "HK",
      "period": 1,
      "section": "Detector",
      "contents": {
        "Event_Count": { "type": "int" },
        "Current_EventID": { "type": "int" },
        "Chamber_Pressure_ADC": { "source": "Chamber_Pressure", "type": "int" },
        "Chamber_Pressure": { "type": "int", "format": "%7.3f", "conversion": convert_Chamber_Pressure },
        "Chamber_Temperature_1_ADC": { "source": "Chamber_Temperature_1", "type": "int" },
        "Chamber_Temperature_1": { "type": "float", "format": "%7.3f", "conversion": convert_RTD_measure },
        "Chamber_Temperature_2_ADC": { "source": "Chamber_Temperature_2", "type": "int" },
        "Chamber_Temperature_2": { "type": "float", "format": "%7.3f", "conversion": convert_RTD_measure },
        "Chamber_Temperature_3_ADC": { "source": "Chamber_Temperature_3", "type": "int" },
        "Chamber_Temperature_3": { "type": "float", "format": "%7.3f", "conversion": convert_RTD_measure },
        "Valve_Temperature_ADC": { "source": "Valve_Temperature", "type": "int" },
        "Valve_Temperature": { "type": "float", "format": "%7.3f", "conversion": convert_RTD_measure },
        "Outer_Temperature_ADC": { "source": "Outer_Temperature", "type": "int" },
        "Outer_Temperature": { "type": "float", "format": "%7.3f", "conversion": convert_RTD_measure },
        "TPC_High_Voltage_Setting": { "source": "TPC_High_Voltage_Setting", "type": "float" },
        "TPC_High_Voltage_Measurement_ADC": { "source": "TPC_High_Voltage_Measurement", "type": "int" },
        "TPC_High_Voltage_Measurement_V": { "type": "float", "format": "%7.3f", "conversion": convert_Slow_ADC },
        "TPC_Current_Measurement_ADC": { "source": "TPC_High_Voltage_Current_Measurement", "type": "int" },
        "TPC_Current_Measurement_uA": { "type": "float", "format": "%7.3f", "source": "TPC_High_Voltage_Current_Measurement", "conversion": function (v) { return convert_Slow_ADC(v) / 4 * 200 } },
        "PMT_High_Voltage_Setting_mV": { "source": "PMT_High_Voltage_Setting", "type": "float", }
      }
    },
    {
      "collection": "grams",
      "directory": "Telemetry",
      "document": "HK",
      "period": 1,
      "section": "DAQ_Vessel",
      "contents": {
        "CPU_Temperature": { "type": "float", "format": "%7.1f", "status": function (v) { return (v > 60) ? "warning" : (v > 80) ? "error" : "safe"; } },
        "HK_Temperature_1": { "type": "float", "format": "%7.3f" },
        "HK_Temperature_2": { "type": "float", "format": "%7.3f" },
        "HK_Temperature_3": { "type": "float", "format": "%7.3f" },
        "HK_Temperature_4": { "type": "float", "format": "%7.3f" },
        "HK_Temperature_5": { "type": "float", "format": "%7.3f" },
        "HK_Humidity_1_percent": { "source": "HK_Humidity_1", "type": "float", "format": "%7.3f" },
        "HK_Humidity_2_percent": { "source": "HK_Humidity_2", "type": "float", "format": "%7.3f" },
        "HK_Humidity_3_percent": { "source": "HK_Humidity_3", "type": "float", "format": "%7.3f" },
        "HK_Humidity_4_percent": { "source": "HK_Humidity_4", "type": "float", "format": "%7.3f" },
        "HK_Humidity_5_percent": { "source": "HK_Humidity_5", "type": "float", "format": "%7.3f" },
        "HK_Pressure_1": { "type": "float", "format": "%7.2f", "conversion": function (v) { return v / 100.0; } },
        "HK_Pressure_2": { "type": "float", "format": "%7.2f", "conversion": function (v) { return v / 100.0; } },
        "HK_Pressure_3": { "type": "float", "format": "%7.2f", "conversion": function (v) { return v / 100.0; } },
        "HK_Pressure_4": { "type": "float", "format": "%7.2f", "conversion": function (v) { return v / 100.0; } },
        "HK_Pressure_5": { "type": "float", "format": "%7.2f", "conversion": function (v) { return v / 100.0; } },
        "Acceleration_x": { "type": "float", "format": "%7.3f" },
        "Acceleration_y": { "type": "float", "format": "%7.3f" },
        "Acceleration_z": { "type": "float", "format": "%7.3f" },
        "Gyro_x": { "type": "float", "format": "%7.3f" },
        "Gyro_y": { "type": "float", "format": "%7.3f" },
        "Gyro_z": { "type": "float", "format": "%7.3f" },
        "Magnet_x_uT": { "source": "Magnet_x", "type": "float", "format": "%7.3f" },
        "Magnet_y_uT": { "source": "Magnet_y", "type": "float", "format": "%7.3f" },
        "Magnet_z_uT": { "source": "Magnet_z", "type": "float", "format": "%7.3f" },
        "Accel_Sensor_Temperature_C": { "source": "Accel_Sensor_Temperature", "type": "float", "format": "%7.3f" },
        "Main_Current_ADC": { "source": "Main_Current", "type": "int" },
        "Main_Current_ADC_Voltage": { "source": "Main_Current", "type": "float", "format": "%7.3f", "conversion": convert_Slow_ADC },
        "Main_Current_A": { "source": "Main_Current", "type": "float", "format": "%7.3f", "conversion": function (v) { return (convert_Slow_ADC(v) - 1) * 1.25; }, "status": function (v) { return (v > 1) ? "warning" : (v > 3) ? "error" : "safe"; } },
        "Main_Voltage_ADC": { "source": "Main_Voltage", "type": "int" },
        "Main_Voltage_ADC_Voltage": { "source": "Main_Voltage", "type": "float", "format": "%7.3f", "conversion": convert_Slow_ADC },
        "Main_Voltage_V": { "source": "Main_Voltage", "type": "float", "format": "%7.3f", "conversion": function (v) { return (convert_Slow_ADC(v)) * 24 / 3.34; } }
      },
    },
    {
      "collection": "grams",
      "directory": "Telemetry",
      "document": "HK",
      "period": 1,
      "section": "Software",
      "contents": {
        "Last_Command_Index": { "type": "int" },
        "Last_Command_Code": { "type": "int" },
        "Last_Command": {
          "source": "Last_Command_Code", "type": "int", "conversion": function (v) {
            return (v in command_code) ? command_code : "None";
          }
        },
        "Command_Reject_Count": { "type": "int" },
        "Software_Error_Code": { "type": "int" },
        "CRC": { "type": "int" },
        "Stop_Code": { "type": "int", "format": "0x%04X" }
      }
    },
    {
      "collection": "grams",
      "directory": "Telemetry",
      "document": "HK",
      "period": 1,
      "section": "Software_Error",
      "contents": {
        "RECEIVE_COMMAND_SERIAL_COMMUNICATION_ERROR": { "type": "string" ,"status":Error_status},
        "RECEIVE_COMMAND_SELECT_ERROR": { "type": "string" ,"status":Error_status},
        "RECEIVE_COMMAND_SREAD_ERROR": { "type": "string" ,"status":Error_status},
        "INVALID_COMMAND": { "type": "string" ,"status":Error_status},
        "PIGPIO_START_ERROR": { "type": "string" ,"status":Error_status},
        "SPI_OPEN_ERROR": { "type": "string" ,"status":Error_status},
        "RTD_DATA_AQUISITION_ERROR_1": { "type": "string" ,"status":Error_status},
        "RTD_DATA_AQUISITION_ERROR_2": { "type": "string" ,"status":Error_status},
        "RTD_DATA_AQUISITION_ERROR_3": { "type": "string" ,"status":Error_status},
        "RTD_DATA_AQUISITION_ERROR_4": { "type": "string" ,"status":Error_status},
        "RTD_DATA_AQUISITION_ERROR_5": { "type": "string" ,"status":Error_status},
        "ENV_DATA_AQUISITION_ERROR_1": { "type": "string" ,"status":Error_status},
        "ENV_DATA_AQUISITION_ERROR_2": { "type": "string" ,"status":Error_status},
        "ENV_DATA_AQUISITION_ERROR_3": { "type": "string" ,"status":Error_status},
        "ENV_DATA_AQUISITION_ERROR_4": { "type": "string" ,"status":Error_status},
        "ENV_DATA_AQUISITION_ERROR_5": { "type": "string" ,"status":Error_status},
        "ACCEL_DEVICE_NOT_FOUND": { "type": "string" ,"status":Error_status},
        "ACCEL_DATA_AQUISITION_ERROR": { "type": "string" ,"status":Error_status},
        "SLOW_ADC_DATA_AQUISITION_ERROR": { "type": "string" ,"status":Error_status},
        "GET_SD_CAPACITY_ERROR": { "type": "string" ,"status":Error_status},
        "ANALOG_DISCOVERY_INITIALIZE_ERROR": { "type": "string" ,"status":Error_status},
        "ANALOG_DISCOVERY_NOT_CONNECTED": { "type": "string" ,"status":Error_status},
        "ONLY_ONE_ANALOG_DISCOVERY_CONNECTED": { "type": "string" ,"status":Error_status},
        "TPC_HV_INVALID_CHANNEL": { "type": "string" ,"status":Error_status},
        "TPC_HV_INVALID_VOLTAGE": { "type": "string" ,"status":Error_status},
        "PMT_HV_INVALID_CHANNEL": { "type": "string" ,"status":Error_status},
        "PMT_HV_INVALID_VOLTAGE": { "type": "string" ,"status":Error_status},
        "TRIGGER_SETUP_ERROR": { "type": "string" ,"status":Error_status},
        "TOO_FEW_EVENTS_DETECTED": { "type": "string" ,"status":Error_status},
        "SEND_TELEMETRY_SERIAL_COMMUNICATION_ERROR": { "type": "string" ,"status":Error_status},
        "SEND_TELEMETRY_SWRITE_ERROR": { "type": "string" ,"status":Error_status},
        "SHUTDOWN_REJECTED": { "type": "string" ,"status":Error_status},
        "REBOOT_REJECTED": { "type": "string" ,"status":Error_status},
        "MODULE_ACCESS_ERROR": { "type": "string" ,"status":Error_status},
        "OTHER_ERRORS": { "type": "string","status":Error_status }
      }
    }
  ];

function convert_Slow_ADC(v) { return (v / 4096 * Va) }
function convert_RTD_measure(v) { return (v / 400 * Rref) / 32.0 - 256 }
function convert_Chamber_Pressure(v) {
  var I = (convert_Slow_ADC(v) / Rshunt) * 1000 // mA
  return (I - 4) * 2 / 16
}

// Status functions
function Error_status(v) {
  return (v == "OK") ? "safe" : (v == "Error" )? "error":"safe";
}

var command_code = {
  100: "Get Status",
  101: "Reset Error",
  102: "Exec Shutdown",
  103: "Exec Reboot",
  104: "Prepare Shutdown",
  105: "Prepare Reboot",
  198: "Prepare Software Stop",
  199: "Exec Software Stop",
  201: "Start Detector Readout",
  202: "Stop Detector Readout",
  203: "Set Trigger Mode",
  204: "Set Trigger Channel",
  205: "Set ADC Offset",
  206: "Exec TPC HV Output",
  207: "Set TPC HV ",
  208: "Exec PMT HV Output",
  209: "Set PMT HV",
  210: "Get Waveform",
  211: "Set Trigger Level",
  212: "Set Trigger Position",
  900: "Dummy 1",
  901: "Dummy 2",
  902: "Dummy 3",
}
