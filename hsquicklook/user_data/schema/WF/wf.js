var data_count = 0
var previous_crc = 0
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
                "Sample_Frequency": {"type": "double", "format": "%7.3f MHz", "source": "Sample_Frequency"},
                "Time_Window": {"type": "double", "format": "%7.3f &micro;s", "source": "Time_Window"}
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
                "Downloaded": { "type": "int", "source": "CRC", "conversion": count_downloaded_data},
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

function count_downloaded_data(v) {
    if (data_count == 8) {
        data_count = 0
        return data_count
    }
    if (previous_crc != v) {
        previous_crc = v
        data_count++
        return data_count
    }
    }
