#!/usr/bin/env ruby

require 'anlnext'
require 'GRAMSBalloon'

class MyApp < ANL::ANLApp
    def setup()
        chain GRAMSBalloon::AnalogDiscoveryManager
        chain GRAMSBalloon::ControlHighVoltage
        with_parameters(device_id: 1, channel: 1)
    end
end

a = MyApp.new
a.run(1000, 1)
