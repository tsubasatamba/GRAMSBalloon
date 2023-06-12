HSQuickLook.main.schema =
    [
        {
            "collection": "grams",
            "directory": "Telemetry",
            "document": "Status",
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
            "document": "Status",
            "period": 1,
            "section": "Status",
            "contents": {
                "Trigger_Mode": { "type": "int", "format": "%08b" },
                "Trigger_Source": {"source": "Trigger_Mode",
                    "type": "string", "conversion": function (v) {
                        var source = v & 0b00001111
                        return (source == 0) ? "Random" : (source == 1) ? "Periodic" : (source == 2) ? "Self" : (source == 11) ? "External" : "Invalid";
                    },
                    "status": function (v) { return (v == "Invalid") ? "error" : "safe"; }
                },
                "Trigger_Mode_slope": {
                    "source": "Trigger_Mode", "type": "string", "conversion": function (v) {
                        var slope = (v & 0b11110000)>>4
                        return (slope == 0) ? "Rise" : (slope == 1) ? "Fall" : (slope == 2) ? "Either" : "Invalid";
                    },
                    "status": function (v) { return (v == "Invalid") ? "error" : "safe"; }
                },
                "Trigger_Device": { "type": "int", "status": function (v) { return (v == 0) ? "safe" : (v == 1) ? "warning" : "error"; } },
                "Trigger_Channel": { "type": "int", "status": function (v) { return (v == 0) ? "safe" : (v == 1) ? "warning" : "error"; } },
                "Trigger_Level": { "type": "float", "format": "%7.3f", "status": function (v) { return (v > 5 || v < -5) ? "error" : "safe"; } },
                "Trigger_Position_us": { "source": "Trigger_Position", "type": "float", "format": "%7.3f", "status": function (v) { return "safe"; } },
                "Sample_Frequency_MHz": { "type": "float", "format": "%7.3f", "source": "Sample_Frequency" },
                "Time_Window_us": { "type": "float", "format": "%7.3f", "source": "Time_Window" },
                "ADC_Offset_1": { "type": "float", "format": "%7.3f" },
                "ADC_Offset_2": { "type": "float", "format": "%7.3f" },
                "ADC_Offset_3": { "type": "float", "format": "%7.3f" },
                "ADC_Offset_4": { "type": "float", "format": "%7.3f" },
                "ADC_Range_1": { "type": "float", "format": "%7.3f" },
                "ADC_Range_2": { "type": "float", "format": "%7.3f" },
                "ADC_Range_3": { "type": "float", "format": "%7.3f" },
                "ADC_Range_4": { "type": "float", "format": "%7.3f" },
                "DAQ_In_Progress": { "type": "string", "conversion": function (v) { return (v == 1) ? "True" : "False"; }, "status": function (v) { return (v == "True") ? "safe" : "error" } },
                "SD_Capacity_GB": { "type": "float", "format": "%7.3f", "source": "SD_Capacity", "status": function (v) { return (v < 100) ? "error" : (v < 200) ? "warning" : "safe"; } }
            }
        },
        {
            "collection": "grams",
            "directory": "Telemetry",
            "document": "Status",
            "period": 1,
            "section": "Footer",
            "contents": {
                "CRC": { "type": "int" },
                "Stop_Code": { "type": "int", "format": "0x%04X" }
            }
        }
    ];
