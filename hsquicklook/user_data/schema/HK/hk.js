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
	      "section": "Detector",
	      "contents": {
          "Event_Count": {"type": "int"},
          "Current_EventID": {"type": "int"},
          "Chamber_Pressure_ADC": {"source": "Chamber_Pressure", "type": "int"},
          "Chamber_Pressure": {"type": "int", "format": "%7.3f", "conversion": convert_Chamber_Pressure},
          "Chamber_Temperature_1_ADC": {"source": "Chamber_Temperature_1", "type": "int"},
          "Chamber_Temperature_1": {"type": "float", "format": "%7.3f", "conversion": convert_RTD_measure},
          "Chamber_Temperature_2_ADC": {"source": "Chamber_Temperature_2", "type": "int"},
          "Chamber_Temperature_2": {"type": "float", "format": "%7.3f", "conversion": convert_RTD_measure},
          "Chamber_Temperature_3_ADC": {"source": "Chamber_Temperature_3", "type": "int"},
          "Chamber_Temperature_3": {"type": "float", "format": "%7.3f", "conversion": convert_RTD_measure},
          "Valve_Temperature_ADC": {"source": "Valve_Temperature", "type": "int"},
          "Valve_Temperature": {"type": "float", "format": "%7.3f", "conversion": convert_RTD_measure},
          "Outer_Temperature_ADC": {"source": "Outer_Temperature", "type": "int"},
          "Outer_Temperature": {"type": "float", "format": "%7.3f", "conversion": convert_RTD_measure},
          "TPC_High_Voltage_Setting": {"source": "TPC_High_Voltage_Setting", "type": "float"},
          "TPC_High_Voltage_Measurement_ADC": {"source": "TPC_High_Voltage_Measurement", "type": "int"},
          "TPC_High_Voltage_Measurement": {"type": "float", "format": "%7.3f", "conversion": convert_Slow_ADC},
          "TPC_Current_Measurement_ADC": {"source": "TPC_High_Voltage_Current_Measurement", "type": "int"},
          "TPC_Current_Measurement": {"type": "float", "format": "%7.3f", "source": "TPC_High_Voltage_Current_Measurement", "conversion": function (v) { return convert_Slow_ADC(v) / 4 * 200 }},
          "PMT_High_Voltage_Setting": {"source": "PMT_High_Voltage_Setting", "type": "float"}
        }
      },
      {
	      "collection": "grams",
	      "directory": "Telemetry",
	      "document": "HK",
	      "period": 1,
	      "section": "DAQ_Vessel",
	      "contents": {
          "CPU_Temperature": {"type": "float","format": "%7.1f"},
          "HK_Temperature_1": {"type": "float","format": "%7.3f"},
          "HK_Temperature_2": {"type": "float","format": "%7.3f"},
          "HK_Temperature_3": {"type": "float","format": "%7.3f"},
          "HK_Temperature_4": {"type": "float","format": "%7.3f"},
          "HK_Temperature_5": {"type": "float","format": "%7.3f"},
          "HK_Humidity_1": {"type": "float","format": "%7.3f"},
          "HK_Humidity_2": {"type": "float","format": "%7.3f"},
          "HK_Humidity_3": {"type": "float","format": "%7.3f"},
          "HK_Humidity_4": {"type": "float","format": "%7.3f"},
          "HK_Humidity_5": {"type": "float","format": "%7.3f"},
          "HK_Pressure_1": {"type": "float","format": "%7.2f","conversion": function(v){return v/100.0;}},
          "HK_Pressure_2": {"type": "float","format": "%7.2f","conversion": function(v){return v/100.0;}},
          "HK_Pressure_3": {"type": "float","format": "%7.2f","conversion": function(v){return v/100.0;}},
          "HK_Pressure_4": {"type": "float","format": "%7.2f","conversion": function(v){return v/100.0;}},
          "HK_Pressure_5": {"type": "float","format": "%7.2f","conversion": function(v){return v/100.0;}},
          "Acceleration_x": {"type": "float","format": "%7.3f"},
          "Acceleration_y": {"type": "float","format": "%7.3f"},
          "Acceleration_z": {"type": "float","format": "%7.3f"},
          "Gyro_x": {"type": "float","format": "%7.3f"},
          "Gyro_y": {"type": "float","format": "%7.3f"},
          "Gyro_z": {"type": "float","format": "%7.3f"},
          "Magnet_x": {"type": "float","format": "%7.3f"},
          "Magnet_y": {"type": "float","format": "%7.3f"},
          "Magnet_z": {"type": "float","format": "%7.3f"},
          "Accel_Sensor_Temperature": {"type": "float", "format": "%7.3f"},
          "Main_Current_ADC": { "source": "Main_Current", "type": "int" },
          "Main_Current_ADC_Voltage": { "source": "Main_Current", "type": "float","format":"%7.3f", "conversion": convert_Slow_ADC},
          "Main_Current": {"type": "float", "format": "%7.3f", "conversion": function(v){ return (convert_Slow_ADC(v)-1)*1.25; }},
          "Main_Voltage_ADC": { "source": "Main_Voltage", "type": "int" },
          "Main_Voltage_ADC_Voltage":{"source":"Main_Voltage","type":"float","format":"%7.3f","conversion":convert_Slow_ADC},
          "Main_Voltage": { "type": "float", "format": "%7.3f", "conversion": function (v) { return (convert_Slow_ADC(v))*24/3.34; } }},
      },
      {
	      "collection": "grams",
	      "directory": "Telemetry",
	      "document": "HK",
	      "period": 1,
	      "section": "Software",
	      "contents": {
          "Last_Command_Index": {"type": "int"},
          "Last_Command_Code": {"type": "int"},
          "Command_Reject_Count": {"type": "int"},
          "Software_Error_Code": {"type": "int"},
          "CRC": {"type": "int"},
          "Stop_Code": {"type": "int", "format": "0x%04X"}
        }
      }
    ];

function convert_Slow_ADC(v) { return (v / 4096 * Va) }
function convert_RTD_measure(v) { return (v / 400 * Rref) / 32.0 - 256 }
function convert_Chamber_Pressure(v) {
  var I = (convert_Slow_ADC(v) / Rshunt) * 1000 // mA
  return (I - 4) * 2 / 16
}
