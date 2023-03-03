#!/usr/bin/env ruby

require 'anlnext'
require 'GRAMSBalloon'

# B57600 = 4097
# B1200 = 9

class MyApp < ANL::ANLApp
    def setup()
        chain GRAMSBalloon::ReceiveCommand
        with_parameters(input_baudrate: 4097, output_baudrate: 4097, serial_path: "/dev/ttyAMA0", length: 1)
    end
end

a = MyApp.new
a.run(100, 1)
