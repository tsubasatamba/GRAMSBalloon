#!/usr/bin/env ruby

require 'anlnext'
require 'GRAMSBalloon'

class MyApp < ANL::ANLApp
    def setup()
        chain GRAMSBalloon::AnalogDiscoveryManager
        chain GRAMSBalloon::RegulateHighVoltage
        with_parameters(device_id: 0, channel: 1)
    end
end

a = MyApp.new
a.run(10, 1)
