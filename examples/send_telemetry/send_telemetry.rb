#!/usr/bin/env ruby
require 'anlnext'
require 'GRAMSBalloon'

class MyApp < ANL::ANLApp
    def setup()
        chain GRAMSBalloon::SendTelemetry
        with_parameters(
            serial_path: "/dev/ttyAMA0",
            save_telemetry: true,
            binary_filename_base: "telemetry_test",
            num_telem_per_file: 10
        )
    end
end

a = MyApp.new
a.run(200, 1)
