#!/usr/bin/env ruby
require 'anlnext'
require 'GRAMSBalloon'

class MyApp < ANL::ANLApp
    def setup()
        chain GRAMSBalloon::AnalogDiscoveryManager
        chain GRAMSBalloon::ReadDAQ
        with_parameters(
            trig_device: 0,
            trig_channel: 0,
            trig_mode: 2,
            trig_level: 1.0,
            trig_position: 0.0,
            time_window: 10.0, # us
            sample_frequency: 2.0, #MHz
            output_filename_base: "DAQ_output",
            num_events_per_file: 100            
        )
    end
end

a = MyApp.new
a.run(200, 1)
