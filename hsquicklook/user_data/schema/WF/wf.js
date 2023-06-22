HSQuickLook.main.schema =
    [
        {
	          "collection": "grams",
	          "directory": "Telemetry",
	          "document": "WF",
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
	          "document": "WF",
	          "period": 1,
	          "section": "Event",
	          "contents": {
                "Event_ID": {"type": "int"},
                "Event_Time": {"type": "int"},
                "Event_Time_us": {"type": "int"},
                "Sample_Frequency_MHz": {"type": "double", "format": "%7.3f", "source": "Sample_Frequency"},
                "Time_Window_us": {"type": "double", "format": "%7.3f", "source": "Time_Window"}
            }
        },
        {
	          "collection": "grams",
	          "directory": "Telemetry",
	          "document": "WF",
	          "period": 1,
	          "section": "Footer",
	          "contents": {
                "CRC": {"type": "int"},
                "Stop_Code": {"type": "int", "format": "0x%04X"}
            }
        },
        {
            "collection": "grams",
            "directory": "Telemetry",
            "document": "waveform_image",
            "period": "4",
            "section": "waveform_all",
            "contents": {
                "waveform_all": {"type": "image"}
            }
        },
        {
            "collection": "grams",
            "directory": "Telemetry",
            "document": "waveform_image",
            "period": "4",
            "section": "waveform_pmt",
            "contents": {
                "waveform_pmt": {"type": "image"}
            }
        }


    ];
