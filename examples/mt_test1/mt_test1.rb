#!/usr/bin/env ruby
require 'anlnext'
require 'GRAMSBalloon'

class MyApp < ANL::ANLApp
    def setup()
        chain GRAMSBalloon::ReceiveCommand
        with_parameters(serial_path: "/dev/ttyAMA0")

        chain GRAMSBalloon::SPIManager
        with_parameters(spi_flags: (1<<5) + (1<<6) + (1<<7) + 1)

        chain GRAMSBalloon::MeasureTemperatureWithRTDSensor
        with_parameters(chip_select: 8)

        chain GRAMSBalloon::GetRaspiStatus

        chain GRAMSBalloon::AnalogDiscoveryManager

        chain GRAMSBalloon::ReadWaveform
        with_parameters(
            trig_device: 0,
            trig_channel: 0,
            trig_mode: 2,
            trig_level: 0.1,
            trig_position: 0.0,
            time_window: 1000.0, # us
            sample_frequency: 2.0, #MHz
            output_filename_base: "DAQ_output",
            num_events_per_file: 100,
            start_reading: true
        )


        chain GRAMSBalloon::SendTelemetry
        with_parameters(
            serial_path: "/dev/ttyAMA0",
            MeasureTemperature_module_names: ["MeasureTemperatureWithRTDSensor"]
        )
    end
end

a = MyApp.new
a.num_parallels = 3
a.modify do |m|
    m.get_parallel_module(0, :ReceiveCommand).off
    m.get_parallel_module(0, :ReadWaveform).off
    for j in 1..2 do
        m.get_parallel_module(j, :SPIManager).off
        m.get_parallel_module(j, :MeasureTemperatureWithRTDSensor).off
        m.get_parallel_module(j, :GetRaspiStatus).off
        m.get_parallel_module(j, :AnalogDiscoveryManager).off
        m.get_parallel_module(j, :SendTelemetry).off
    end
    m.get_parallel_module(1, :ReceiveCommand).off
    m.get_parallel_module(2, :ReadWaveform).off
end
a.run(1000000, 100)
