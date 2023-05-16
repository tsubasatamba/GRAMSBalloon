#!/usr/bin/env ruby
require 'anlnext'
require 'GRAMSBalloon'

class MyApp < ANL::ANLApp
    def setup()
=begin
        chain GRAMSBalloon::AnalogDiscoveryManager
        chain GRAMSBalloon::ReadDAQ
        with_parameters(
            trig_device: 0,
            trig_channel: 0,
            trig_mode: 2,
            trig_level: 0.1,
            trig_position: 0.0,
            time_window: 90.0, # us
            sample_frequency: 2.0, #MHz
            output_filename_base: "DAQ_output",
            num_events_per_file: 5
        )
=end
        chain GRAMSBalloon::SendTelemetry
        with_parameters(
            serial_path: "/dev/ttyAMA0"
        )
    end
end

a = MyApp.new
a.run(200, 1)
