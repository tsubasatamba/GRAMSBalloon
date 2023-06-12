var Va = 5.026 //[V] for slow ADC
var Rref = 430 //[ohm] for RTD measurement
var Rshunt = 100 //[ohm] for Pressure gauge

var accel_x = 0
var accel_y = 0
var accel_z = 0

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
        "Event_Count": { "type": "int", "status": "safe" },
        "Current_EventID": { "type": "int", "status": "safe" },
        "Chamber_Pressure_ADC": { "source": "Chamber_Pressure", "type": "int" },
        "Chamber_Pressure": { "type": "int", "format": "%7.3f", "conversion": convert_Chamber_Pressure, "status": function (v) { return status_func("Chamber_Pressure", v); } },
        "Chamber_Temperature_Upper_ADC": { "source": "Chamber_Temperature_1", "type": "int" },
        "Chamber_Temperature_Upper": { "source": "Chamber_Temperature_1", "type": "float", "format": "%7.3f", "conversion": convert_RTD_measure, "status": function (v) { return status_func("Chamber_Temperature", v); } },
        "Chamber_Temperature_Middle_ADC": { "source": "Chamber_Temperature_3", "type": "int" },
        "Chamber_Temperature_Middle": { "source": "Chamber_Temperature_2", "type": "float", "format": "%7.3f", "conversion": convert_RTD_measure, "status": function (v) { return status_func("Chamber_Temperature", v); } },
        "Chamber_Temperature_Lower_ADC": { "source": "Chamber_Temperature_3", "type": "int" },
        "Chamber_Temperature_Lower": { "source": "Chamber_Temperature_3", "type": "float", "format": "%7.3f", "conversion": convert_RTD_measure, "status": function (v) { return status_func("Chamber_Temperature", v); } },
        "Valve_Temperature_ADC": { "source": "Valve_Temperature", "type": "int" },
        "Valve_Temperature": { "type": "float", "format": "%7.3f", "conversion": convert_RTD_measure, "status": function (v) { return status_func("Valve_Temperature", v); } },
        "Outer_Temperature_ADC": { "source": "Outer_Temperature", "type": "int" },
        "Outer_Temperature": { "type": "float", "format": "%7.3f", "conversion": convert_RTD_measure, "status": function (v) { return status_func("Outer_Temperature", v); } },
        "TPC_High_Voltage_Setting": { "source": "TPC_High_Voltage_Setting", "type": "float", "status": function (v) { return status_func("TPC_High_Voltage_Setting", v); } },
        "TPC_High_Voltage_Measurement_ADC": { "source": "TPC_High_Voltage_Measurement", "type": "int" },
        "TPC_High_Voltage_Measurement_kV": { "source": "TPC_High_Voltage_Measurement", "type": "float", "format": "%7.3f", "conversion": function (v) { return convert_Slow_ADC(v) * 4; }, "status": function (v) { return status_func("TPC_High_Voltage_Measurement", v); } },
        "TPC_Current_Measurement_ADC": { "source": "TPC_High_Voltage_Current_Measurement", "type": "int" },
        "TPC_Current_Measurement_uA": {
          "type": "float", "format": "%7.3f", "source": "TPC_High_Voltage_Current_Measurement", "conversion": function (v) { return convert_Slow_ADC(v) / 4 * 200 }, "status": function (v) { return status_func("TPC_Current_Measurement", v); }
        },
        "PMT_High_Voltage_Setting_V": { "source": "PMT_High_Voltage_Setting", "type": "float", "status": function (v) { return status_func("PMT_High_Voltage_Setting", v); } }
      }
    },
    {
      "collection": "grams",
      "directory": "Telemetry",
      "document": "HK",
      "period": 1,
      "section": "DAQ_Vessel",
      "contents": {
        "CPU_Temperature": { "type": "float", "format": "%7.1f", "status": function (v) { return status_func("CPU_Temperature", v); } },
        "HK_Temperature_Upper": { "source": "HK_Temperature_3", "type": "float", "format": "%7.3f", "status": function (v) { return status_func("HK_Temperature", v); } },
        "HK_Temperature_Middle": { "source": "HK_Temperature_4", "type": "float", "format": "%7.3f", "status": function (v) { return status_func("HK_Temperature", v); } },
        "HK_Temperature_Lower": { "source": "HK_Temperature_5", "type": "float", "format": "%7.3f", "status": function (v) { return status_func("HK_Temperature", v); } },

        "HK_Humidity_Upper": { "source": "HK_Humidity_3", "type": "float", "format": "%7.3f", "status": function (v) { return status_func("HK_Humidity", v); } },
        "HK_Humidity_Middle": { "source": "HK_Humidity_4", "type": "float", "format": "%7.3f", "status": function (v) { return status_func("HK_Humidity", v); } },
        "HK_Humidity_Lower": { "source": "HK_Humidity_5", "type": "float", "format": "%7.3f", "status": function (v) { return status_func("HK_Humidity", v); } },

        "HK_Pressure_Upper": { "source": "HK_Pressure_3", "type": "float", "format": "%7.2f", "conversion": function (v) { return v / 100.0; }, "status": function (v) { return status_func("HK_Pressure", v); } },
        "HK_Pressure_Middle": { "source": "HK_Pressure_4", "type": "float", "format": "%7.2f", "conversion": function (v) { return v / 100.0; }, "status": function (v) { return status_func("HK_Pressure", v); } },
        "HK_Pressure_Lower": { "source": "HK_Pressure_5", "type": "float", "format": "%7.2f", "conversion": function (v) { return v / 100.0; }, "status": function (v) { return status_func("HK_Pressure", v); } },
        "Acceleration_x": {
          "type": "float", "format": "%7.3f", "conversion": function (v) {
            accel_x = v;
            return v;
          }, "status": function (v) { return status_func("Acceleration", v); }
        },
        "Acceleration_y": {
          "type": "float", "format": "%7.3f", "conversion": function (v) {
            accel_y = v;
            return v;
          }, "status": function (v) { return status_func("Acceleration", v); }
        },
        "Acceleration_z": {
          "type": "float", "format": "%7.3f", "conversion": function (v) {
            accel_z = v;
            return v;
          }, "status": function (v) { return status_func("Acceleration", v); }
        },
        "Acceleration": {
          "type": "float", "format": "%7.3f", "conversion": function (v) {
            return Math.sqrt(accel_x * accel_x + accel_y * accel_y + accel_z * accel_z);
          }

        },
        "Gyro_x": { "type": "float", "format": "%7.3f", "status": function (v) { return status_func("Gyro", v); } },
        "Gyro_y": { "type": "float", "format": "%7.3f", "status": function (v) { return status_func("Gyro", v); } },
        "Gyro_z": { "type": "float", "format": "%7.3f", "status": function (v) { return status_func("Gyro", v); } },
        "Magnet_x_uT": { "source": "Magnet_x", "type": "float", "format": "%7.3f", "status": function (v) { return status_func("Magnet", v); } },
        "Magnet_y_uT": { "source": "Magnet_y", "type": "float", "format": "%7.3f", "status": function (v) { return status_func("Magnet", v); } },
        "Magnet_z_uT": { "source": "Magnet_z", "type": "float", "format": "%7.3f", "status": function (v) { return status_func("Magnet", v); } },
        "Accel_Sensor_Temperature_C": { "source": "Accel_Sensor_Temperature", "type": "float", "format": "%7.3f", "status": function (v) { return status_func("HK_Temperature", v); } },
        "Main_Current_ADC": { "source": "Main_Current", "type": "int" },
        "Main_Current_ADC_Voltage": { "source": "Main_Current", "type": "float", "format": "%7.3f", "conversion": convert_Slow_ADC },
        "Main_Current_A": { "source": "Main_Current", "type": "float", "format": "%7.3f", "conversion": function (v) { return (convert_Slow_ADC(v) - 1) * 1.25; }, "status": function (v) { return status_func("Main_Current", v); } },
        "Main_Voltage_ADC": { "source": "Main_Voltage", "type": "int" },
        "Main_Voltage_ADC_Voltage": { "source": "Main_Voltage", "type": "float", "format": "%7.3f", "conversion": convert_Slow_ADC },
        "Main_Voltage_V": { "source": "Main_Voltage", "type": "float", "format": "%7.3f", "conversion": function (v) { return (convert_Slow_ADC(v)) * 24 / 3.34; }, "status": function (v) { return status_func("Main_Voltage", v); } }
      },
    },
    {
      "collection": "grams",
      "directory": "Telemetry",
      "document": "HK",
      "period": 1,
      "section": "Software",
      "contents": {
        "Last_Command_Index": { "type": "int", "status": "safe" },
        "Last_Command_Code": { "type": "int" },
        "Last_Command": {
          "source": "Last_Command_Code", "type": "string", "conversion": function (v) {
            return (v in command_code) ? command_code[v] : "None";
          },
          "status": function (v) { return (v == "None") ? "error" : "safe" }
        },
        "Command_Reject_Count": { "type": "int", "status": function (v) { return (v > 0) ? "warning" : "safe"; } },
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
        "RECEIVE_COMMAND_SERIAL_COMMUNICATION_ERROR": { "type": "string", "status": Error_status },
        "RECEIVE_COMMAND_SELECT_ERROR": { "type": "string", "status": Error_status },
        "RECEIVE_COMMAND_SREAD_ERROR": { "type": "string", "status": Error_status },
        "INVALID_COMMAND": { "type": "string", "status": Error_status },
        "PIGPIO_START_ERROR": { "type": "string", "status": Error_status },
        "SPI_OPEN_ERROR": { "type": "string", "status": Error_status },
        "RTD_DATA_AQUISITION_ERROR_1": { "type": "string", "status": Error_status },
        "RTD_DATA_AQUISITION_ERROR_2": { "type": "string", "status": Error_status },
        "RTD_DATA_AQUISITION_ERROR_3": { "type": "string", "status": Error_status },
        "RTD_DATA_AQUISITION_ERROR_4": { "type": "string", "status": Error_status },
        "RTD_DATA_AQUISITION_ERROR_5": { "type": "string", "status": Error_status },
        "ENV_DATA_AQUISITION_ERROR_1": { "type": "string", "status": Error_status },
        "ENV_DATA_AQUISITION_ERROR_2": { "type": "string", "status": Error_status },
        "ENV_DATA_AQUISITION_ERROR_3": { "type": "string", "status": Error_status },
        "ENV_DATA_AQUISITION_ERROR_4": { "type": "string", "status": Error_status },
        "ENV_DATA_AQUISITION_ERROR_5": { "type": "string", "status": Error_status },
        "ACCEL_DEVICE_NOT_FOUND": { "type": "string", "status": Error_status },
        "ACCEL_DATA_AQUISITION_ERROR": { "type": "string", "status": Error_status },
        "SLOW_ADC_DATA_AQUISITION_ERROR": { "type": "string", "status": Error_status },
        "GET_SD_CAPACITY_ERROR": { "type": "string", "status": Error_status },
        "ANALOG_DISCOVERY_INITIALIZE_ERROR": { "type": "string", "status": Error_status },
        "ANALOG_DISCOVERY_NOT_CONNECTED": { "type": "string", "status": Error_status },
        "ONLY_ONE_ANALOG_DISCOVERY_CONNECTED": { "type": "string", "status": Error_status },
        "TPC_HV_INVALID_CHANNEL": { "type": "string", "status": Error_status },
        "TPC_HV_INVALID_VOLTAGE": { "type": "string", "status": Error_status },
        "PMT_HV_INVALID_CHANNEL": { "type": "string", "status": Error_status },
        "PMT_HV_INVALID_VOLTAGE": { "type": "string", "status": Error_status },
        "TRIGGER_SETUP_ERROR": { "type": "string", "status": Error_status },
        "TOO_FEW_EVENTS_DETECTED": { "type": "string", "status": Error_status },
        "SEND_TELEMETRY_SERIAL_COMMUNICATION_ERROR": { "type": "string", "status": Error_status },
        "SEND_TELEMETRY_SWRITE_ERROR": { "type": "string", "status": Error_status },
        "SHUTDOWN_REJECTED": { "type": "string", "status": Error_status },
        "REBOOT_REJECTED": { "type": "string", "status": Error_status },
        "MODULE_ACCESS_ERROR": { "type": "string", "status": Error_status },
        "OTHER_ERRORS": { "type": "string", "status": Error_status }
      }
    },
    {
      "collection": "grams",
      "directory": "Telemetry",
      "document": "HK",
      "period": 1,
      "section": "Out_Of_Use",
      "contents": {
        "HK_Temperature_1": { "type": "float", "format": "%7.3f" },
        "HK_Temperature_2": { "type": "float", "format": "%7.3f" },
        "HK_Humidity_1_percent": { "source": "HK_Humidity_1", "type": "float", "format": "%7.3f" },
        "HK_Humidity_2_percent": { "source": "HK_Humidity_2", "type": "float", "format": "%7.3f" },
        "HK_Pressure_1": { "type": "float", "format": "%7.2f", "conversion": function (v) { return v / 100.0; } },
        "HK_Pressure_2": { "type": "float", "format": "%7.2f", "conversion": function (v) { return v / 100.0; } },
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
  return (v == "OK") ? "safe" : (v == "Error") ? "error" : "safe";
}

var status_configuration = {
  "Chamber_Pressure": { "error_ranges": [[Infinity, Infinity], [-Infinity, 0]], "warning_ranges": [[Infinity, Infinity]] },
  "Chamber_Temperature": { "error_ranges": [[-Infinity, -256]], "warning_ranges": [[Infinity, Infinity]] },
  "Outer_Temperature": { "error_ranges": [[-Infinity, -256]], "warning_ranges": [[Infinity, Infinity]] },
  "Valve_Temperature": { "error_ranges": [[-Infinity, -256]], "warning_ranges": [[Infinity, Infinity]] },
  "TPC_High_Voltage_Setting": { "error_ranges": [[0.1, Infinity]], "warning_ranges": [[Infinity, Infinity]] },
  "TPC_High_Voltage_Measurement": { "error_ranges": [[Infinity, Infinity]], "warning_ranges": [[Infinity, Infinity]] },
  "TPC_Current_Measurement": { "error_ranges": [[Infinity, Infinity]], "warning_ranges": [[Infinity, Infinity]] },
  "PMT_High_Voltage_Setting": { "error_ranges": [[Infinity, Infinity]], "warning_ranges": [[Infinity, Infinity]] },
  "CPU_Temperature": { "error_ranges": [[80, Infinity]], "warning_ranges": [[50, 80]] },
  "HK_Humidity": { "error_ranges": [[Infinity, Infinity]], "warning_ranges": [[Infinity, Infinity]] },
  "HK_Temperature": { "error_ranges": [[Infinity, Infinity]], "warning_ranges": [[Infinity, Infinity]] },
  "HK_Pressure": { "error_ranges": [[Infinity, Infinity]], "warning_ranges": [[Infinity, Infinity]] },
  "Acceleration": { "error_ranges": [[Infinity, Infinity]], "warning_ranges": [[Infinity, Infinity]] },
  "Gyro": { "error_ranges": [[Infinity, Infinity]], "warning_ranges": [[Infinity, Infinity]] },
  "Magnet": { "error_ranges": [[Infinity, Infinity]], "warning_ranges": [[Infinity, Infinity]] },
  "Main_Current": { "error_ranges": [[3, Infinity], [-Infinity, 0]], "warning_ranges": [[1, 3]] },
  "Main_Voltage": { "error_ranges": [[0, 18], [36, Infinity]], "warning_ranges": [[34, 36], [18, 20]] },
}

function status_func(name, v) {
  for (var i = 0; i < status_configuration[name]["error_ranges"].length; i++) {
    if ((status_configuration[name]["error_ranges"][i][1] >= v) && (status_configuration[name]["error_ranges"][i][0] <= v)) {
      // console.log("error in " + String(name) + "\nupper: " + String(status_configuration[name]["error_range"] + "\nlower: " + String(status_configuration[name]["error_range_lower"]) + "\nValue: " + String(v)));
      return "error";
    }
  }
  for (var i = 0; i < status_configuration[name]["warning_ranges"].length; i++) {
    if ((status_configuration[name]["warning_ranges"][i][1] >= v) && (status_configuration[name]["warning_ranges"][i][0] <= v)) {
      return "warning";
    }
  }
  return "safe";
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
