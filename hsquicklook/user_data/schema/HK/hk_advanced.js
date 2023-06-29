var Va = 5.026 //[V] for slow ADC
var Rref = 430 //[ohm] for RTD measurement
var Rshunt = 100 //[ohm] for Pressure gauge
var error_TPC_measurement = 0.2


//Global variables
var accel_x = 0
var accel_y = 0
var accel_z = 0
var TPC_setting = 0

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
                "Event_Rate" : {"type": "float", "status": "safe" , "format": "%7.3f"},
                "Current_EventID": { "type": "int", "status": "safe" },
                "Chamber_Pressure": { "type": "int", "format": "%7.3f atm", "conversion": convert_Chamber_Pressure, "status": function (v) { return status_func("Chamber_Pressure", v); } },
                "Chamber_Temperature_Upper": { "source": "Chamber_Temperature_1", "type": "float", "format": "%7.3f &deg;C", "conversion": convert_RTD_measure, "status": function (v) { return status_func("Chamber_Temperature", v); } },
                "Chamber_Temperature_Middle": { "source": "Chamber_Temperature_2", "type": "float", "format": "%7.3f &deg;C", "conversion": convert_RTD_measure, "status": function (v) { return status_func("Chamber_Temperature_Middle", v); } },
                "Chamber_Temperature_Lower": { "source": "Chamber_Temperature_3", "type": "float", "format": "%7.3f &deg;C", "conversion": convert_RTD_measure, "status": function (v) { return status_func("Chamber_Temperature", v); } },
                "Valve_Temperature": { "type": "float", "format": "%7.3f &deg;C", "conversion": convert_RTD_measure, "status": function (v) { return status_func("Valve_Temperature", v); } },
                "Outer_Temperature": { "type": "float", "format": "%7.3f &deg;C", "conversion": convert_RTD_measure, "status": function (v) { return status_func("Outer_Temperature", v); } },
                "TPC_Control_Voltage_Setting": { "source": "TPC_High_Voltage_Setting", "type": "float", "format": "%7.3f V", "status": function (v) { return status_func("TPC_High_Voltage_Setting", v); } },
                "TPC_High_Voltage_Setting": {
                    "source": "TPC_High_Voltage_Setting", "type": "float", "format": "%7.3f kV", "conversion": function (v) { return v * 2; },
                    "status": function (v) {
                        TPC_setting = v;
                        return status_func("TPC_High_Voltage_Setting", v);
                    }
                },
                "TPC_High_Voltage_Measurement_ADC": { "source": "TPC_High_Voltage_Measurement", "type": "int" },
                "TPC_High_Voltage_Measurement": {
                    "source": "TPC_High_Voltage_Measurement", "type": "float", "format": "%7.3f kV", "conversion": function (v) { return convert_Slow_ADC(v) * 4; },
                    "status": function (v) {
                        if ((TPC_setting - v) >= error_TPC_measurement) {
                            return "error";
                        }
                        return "safe";
                    }
                },
                "TPC_Current_Measurement_ADC": { "source": "TPC_High_Voltage_Current_Measurement", "type": "int" },
                "TPC_Current_Measurement": {
                    "type": "float", "format": "%7.3f uA", "source": "TPC_High_Voltage_Current_Measurement", "conversion": function (v) { return convert_Slow_ADC(v) / 4 * 200 }, "status": function (v) { return status_func("TPC_Current_Measurement", v); }
                },
                "PMT_Control_Voltage_Setting": { "source": "PMT_High_Voltage_Setting", "type": "float", "format": "%7.3f V", "status": function (v) { return status_func("PMT_High_Voltage_Setting", v); } },
                "PMT_High_Voltage_Setting": { "source": "PMT_High_Voltage_Setting", "type": "float", "format": "%7.0f V", "conversion": convert_PMT_control_voltage, "status": function (v) { return status_func("PMT_High_Voltage_Setting", v); } }
            }
        },
        {
            "collection": "grams",
            "directory": "Telemetry",
            "document": "HK",
            "period": 1,
            "section": "DAQ_Vessel",
            "contents": {
                "CPU_Temperature": { "type": "float", "format": "%7.1f &deg;C", "status": function (v) { return status_func("CPU_Temperature", v); } },
                "HK_Temperature_Upper": { "source": "HK_Temperature_3", "type": "float", "format": "%7.3f &deg;C", "status": function (v) { return status_func("HK_Temperature", v); } },
                "HK_Temperature_Middle": { "source": "HK_Temperature_4", "type": "float", "format": "%7.3f &deg;C", "status": function (v) { return status_func("HK_Temperature", v); } },
                "HK_Temperature_Lower": { "source": "HK_Temperature_5", "type": "float", "format": "%7.3f &deg;C", "status": function (v) { return status_func("HK_Temperature", v); } },

                "HK_Humidity_Upper": { "source": "HK_Humidity_3", "type": "float", "format": "%7.3f &percnt;", "status": function (v) { return status_func("HK_Humidity", v); } },
                "HK_Humidity_Middle": { "source": "HK_Humidity_4", "type": "float", "format": "%7.3f &percnt;", "status": function (v) { return status_func("HK_Humidity", v); } },
                "HK_Humidity_Lower": { "source": "HK_Humidity_5", "type": "float", "format": "%7.3f &percnt;", "status": function (v) { return status_func("HK_Humidity", v); } },

                "HK_Pressure_Upper": { "source": "HK_Pressure_3", "type": "float", "format": "%7.2f atm", "conversion": function (v) { return v / 100.0 / 1013.25; }, "status": function (v) { return status_func("HK_Pressure", v); } },
                "HK_Pressure_Middle": { "source": "HK_Pressure_4", "type": "float", "format": "%7.2f atm", "conversion": function (v) { return v / 100.0 / 1013.25; }, "status": function (v) { return status_func("HK_Pressure", v); } },
                "HK_Pressure_Lower": { "source": "HK_Pressure_5", "type": "float", "format": "%7.2f atm", "conversion": function (v) { return v / 100.0 / 1013.25; }, "status": function (v) { return status_func("HK_Pressure", v); } },
                "Acceleration_x": {
                    "type": "float", "format": "%7.3f G", "conversion": function (v) {
                        accel_x = v;
                        return v;
                    }, "status": function (v) { return status_func("Acceleration", v); }
                },
                "Acceleration_y": {
                    "type": "float", "format": "%7.3f G", "conversion": function (v) {
                        accel_y = v;
                        return v;
                    }, "status": function (v) { return status_func("Acceleration", v); }
                },
                "Acceleration_z": {
                    "type": "float", "format": "%7.3f G", "conversion": function (v) {
                        accel_z = v;
                        return v;
                    }, "status": function (v) { return status_func("Acceleration", v); }
                },
                "Acceleration": {
                    "source": "Acceleration_x", "type": "float", "format": "%7.3f G", "conversion": function (v) {
                        return Math.sqrt(accel_x * accel_x + accel_y * accel_y + accel_z * accel_z);
                    }

                },
                "Gyro_x": { "type": "float", "format": "%7.3f", "status": function (v) { return status_func("Gyro", v); } },
                "Gyro_y": { "type": "float", "format": "%7.3f", "status": function (v) { return status_func("Gyro", v); } },
                "Gyro_z": { "type": "float", "format": "%7.3f", "status": function (v) { return status_func("Gyro", v); } },
                "Magnet_x": { "source": "Magnet_x", "type": "float", "format": "%7.3f uT", "status": function (v) { return status_func("Magnet", v); } },
                "Magnet_y": { "source": "Magnet_y", "type": "float", "format": "%7.3f uT", "status": function (v) { return status_func("Magnet", v); } },
                "Magnet_z": { "source": "Magnet_z", "type": "float", "format": "%7.3f uT", "status": function (v) { return status_func("Magnet", v); } },
                "Accel_Sensor_Temperature": { "source": "Accel_Sensor_Temperature", "type": "float", "format": "%7.3f &deg;C", "status": function (v) { return status_func("HK_Temperature", v); } },
                "Main_Current": { "source": "Main_Current", "type": "float", "format": "%7.3f A", "conversion": function (v) { return (convert_Slow_ADC(v) - 1) * 1.25; }, "status": function (v) { return status_func("Main_Current", v); } },
                "Main_Voltage": { "source": "Main_Voltage", "type": "float", "format": "%7.3f V", "conversion": function (v) { return (convert_Slow_ADC(v)) * 24 / 3.34; }, "status": function (v) { return status_func("Main_Voltage", v); } }
            },
        },
        {
            "collection": "grams",
            "directory": "Telemetry",
            "document": "HK",
            "period": 1,
            "section": "Software",
            "tableName": "Command_Name",
            "contents": {
                "Last_Command": {
                    "source": "Last_Command_Code", "type": "string", "conversion": function (v) {
                        return (v in command_code) ? command_code[v] : "None";
                    },
                    "status": function (v) { return (v == "None") ? "error" : "safe" },
                    "format": "&nbsp;%s"
                }
            }
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
                "Command_Reject_Count": { "type": "int", "status": function (v) { return (v > 0) ? "warning" : "safe"; } },
                "Software_Error_Code": { "type": "int", "status": function (v) { return (v > 0) ? "warning" : "safe"; } },
                "CRC": { "type": "int" },
                "Stop_Code": { "type": "int", "format": "0x%04X" }
            }
        },
        {
            "collection": "grams",
            "directory": "Telemetry",
            "document": "HK",
            "period": 1,
            "tableName": "Trend_graph",
            "section": "Detector",
            "contents": {
                "Chamber_Temperature": { "type": "trend-graph",
                    "group": [
                        {"source": "Chamber_Temperature_1","conversion":convert_RTD_measure, "options":{"legend": "Upper","color": "black"}},
                        {"source": "Chamber_Temperature_2","conversion":convert_RTD_measure, "options":{"legend": "Middle","color": "red"}},
                        {"source": "Chamber_Temperature_3","conversion":convert_RTD_measure, "options":{"legend": "Lower","color": "green"}},
                    ],
                    "options":{"xWidth": 1000,"yRange":[-200, 30]}
                },
                "Pressure": {
                    "type": "trend-graph",
                    "group":[
                        {"source": "Chamber_Pressure","conversion":convert_Chamber_Pressure,"options":{"legend":"Chamber","color":"red"}},
                    ],
                    "options":{"xWidth": 1000,"yRange":[0.9, 1.5]}

                }
            }
        }
    ];

function convert_Slow_ADC(v) { return (v / 4096 * Va) }
function convert_RTD_measure(v) { return (v / 400 * Rref) / 32.0 - 256 }
function convert_Chamber_Pressure(v) {
    var I = (convert_Slow_ADC(v) / Rshunt) * 1000 // mA
    return (I - 4) * 2 / 16
}
function convert_PMT_control_voltage(v) { return v / 5 * 1250; }

// Status functions
function Error_status(v) {
    return (v == "OK") ? "safe" : (v == "Error") ? "error" : "safe";
}

var status_configuration = {
    "Chamber_Pressure": { "error_ranges": [[1.5, Infinity], [-Infinity, 0]], "warning_ranges": [[1.3, Infinity], [-Infinity, 0.99]] },
    "Chamber_Temperature_Middle": { "error_ranges": [[-180, Infinity], [-Infinity, -256]], "warning_ranges": [[Infinity, Infinity]] },
    "Chamber_Temperature": { "error_ranges": [[-Infinity, -256]], "warning_ranges": [[Infinity, Infinity]] },
    "Outer_Temperature": { "error_ranges": [[-Infinity, -256]], "warning_ranges": [[Infinity, Infinity]] },
    "Valve_Temperature": { "error_ranges": [[-Infinity, -256]], "warning_ranges": [[Infinity, Infinity]] },
    "TPC_High_Voltage_Setting": { "error_ranges": [[0.1, Infinity]], "warning_ranges": [[Infinity, Infinity]] },
    "TPC_High_Voltage_Measurement": { "error_ranges": [[Infinity, Infinity]], "warning_ranges": [[Infinity, Infinity]] },
    "TPC_Current_Measurement": { "error_ranges": [[Infinity, Infinity]], "warning_ranges": [[Infinity, Infinity]] },
    "PMT_High_Voltage_Setting": { "error_ranges": [[Infinity, Infinity]], "warning_ranges": [[Infinity, Infinity]] },
    "PMT_High_Voltage_Setting_": { "error_ranges": [[Infinity, Infinity]], "warning_ranges": [[Infinity, Infinity]] },
    "CPU_Temperature": { "error_ranges": [[80, Infinity]], "warning_ranges": [[60, 80]] },
    "HK_Humidity": { "error_ranges": [[10, Infinity]], "warning_ranges": [[Infinity, Infinity]] },
    "HK_Temperature": { "error_ranges": [[50, Infinity], [-Infinity, -20]], "warning_ranges": [[40, Infinity], [-Infinity, 0]] },
    "HK_Pressure": { "error_ranges": [[2000 / 1013.25, Infinity], [-Infinity, 800 / 1013.25]], "warning_ranges": [[1500 / 1013.25, Infinity], [-Infinity, 900 / 1013.25]] },
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
    207: "Set TPC HV",
    208: "Exec PMT HV Output",
    209: "Set PMT HV",
    210: "Get Waveform",
    211: "Set Trigger Level",
    212: "Set Trigger Position",
    301: "Set TPC HV Upper Limit",
    302: "Set PMT HV Upper Limit",
    900: "Dummy 1",
    901: "Dummy 2",
    902: "Dummy 3",
}
