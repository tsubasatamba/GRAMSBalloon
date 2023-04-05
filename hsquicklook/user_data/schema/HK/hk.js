HSQuickLook.main.schema =
    [
      {
	      "collection": "grams",
	      "directory": "Telemetry",
	      "document": "HK",
	      "period": 1,
	      "section": "Header",
	      "contents": {
          "Start": {"type": "int", "format": "0x%04X"},
          "Telemetry_Type": {"type": "int"},
          "Time": {"type": "int"},
          "Time_us": {"type": "int"},
          "Telemetry_Index": {"type": "int"}
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
          "Trigger_Count": {"type": "int"},
          "Chamber_Puressure_ADC": {"source": "Chamber_Puressure", "type": "int"},
          "Chamber_Puressure": {"type": "int", "format": "%7.3f", "conversion": function(v){ return (v*0.1); }},
          "Chamber_Temperature_1_ADC": {"source": "Chamber_Temperature_1", "type": "int"},
          "Chamber_Temperature_1": {"type": "int", "format": "%7.3f", "conversion": function(v){ return (v*0.1); }},
          "Chamber_Temperature_2_ADC": {"source": "Chamber_Temperature_2", "type": "int"},
          "Chamber_Temperature_2": {"type": "int", "format": "%7.3f", "conversion": function(v){ return (v*0.1); }},
          "Chamber_Temperature_3_ADC": {"source": "Chamber_Temperature_3", "type": "int"},
          "Chamber_Temperature_3": {"type": "int", "format": "%7.3f", "conversion": function(v){ return (v*0.1); }},
          "Valve_Temperature_ADC": {"source": "Valve_Temperature", "type": "int"},
          "Valve_Temperature": {"type": "int", "format": "%7.3f", "conversion": function(v){ return (v*0.1); }},
          "Outer_Temperature_ADC": {"source": "Outer_Temperature", "type": "int"},
          "Outer_Temperature": {"type": "int", "format": "%7.3f", "conversion": function(v){ return (v*0.1); }},
          "TPC_High_Voltage_Setting_ADC": {"source": "TPC_High_Voltage_Setting", "type": "int"},
          "TPC_High_Voltage_Setting": {"type": "int", "format": "%7.3f", "conversion": function(v){ return (v*0.1); }},
          "TPC_High_Voltage_Measurement_ADC": {"source": "TPC_High_Voltage_Measurement", "type": "int"},
          "TPC_High_Voltage_Measurement": {"type": "int", "format": "%7.3f", "conversion": function(v){ return (v*0.1); }},
          "PMT_High_Voltage_Setting_ADC": {"source": "PMT_High_Voltage_Setting", "type": "int"},
          "PMT_High_Voltage_Setting": {"type": "int", "format": "%7.3f", "conversion": function(v){ return (v*0.1); }},
          "PMT_High_Voltage_Measurement_ADC": {"source": "PMT_High_Voltage_Measurement", "type": "int"},
          "PMT_High_Voltage_Measurement": {"type": "int", "format": "%7.3f", "conversion": function(v){ return (v*0.1); }}
        }
      },
      {
	      "collection": "grams",
	      "directory": "Telemetry",
	      "document": "HK",
	      "period": 1,
	      "section": "DAQ_Vessel",
	      "contents": {
          "CPU_Temperature": {"type": "int"},
          "HK_Temperature_1": {"type": "int"},
          "HK_Temperature_2": {"type": "int"},
          "HK_Temperature_3": {"type": "int"},
          "HK_Temperature_4": {"type": "int"},
          "HK_Temperature_5": {"type": "int"},
          "HK_Humidity_1": {"type": "int"},
          "HK_Humidity_2": {"type": "int"},
          "HK_Humidity_3": {"type": "int"},
          "HK_Humidity_4": {"type": "int"},
          "HK_Humidity_5": {"type": "int"},
          "HK_Pressure_1": {"type": "int"},
          "HK_Pressure_2": {"type": "int"},
          "HK_Pressure_3": {"type": "int"},
          "HK_Pressure_4": {"type": "int"},
          "HK_Pressure_5": {"type": "int"},
          "Acceleration_1": {"type": "int"},
          "Acceleration_2": {"type": "int"},
          "Acceleration_3": {"type": "int"},
          "Acceleration_4": {"type": "int"},
          "Acceleration_5": {"type": "int"},
          "Acceleration_6": {"type": "int"},
          "Acceleration_7": {"type": "int"},
          "Acceleration_8": {"type": "int"},
          "Acceleration_9": {"type": "int"},
          "Main_Current_ADC": {"source": "Main_Current", "type": "int"},
          "Main_Current": {"type": "int", "format": "%7.3f", "conversion": function(v){ return (v*0.1); }},
          "Main_Voltage_ADC": {"source": "Main_Voltage", "type": "int"},
          "Main_Voltage": {"type": "int", "format": "%7.3f", "conversion": function(v){ return (v*0.1); }}
        }
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
          "Software_Error_Code": {"type": "int"}
        }
      }      
    ];
