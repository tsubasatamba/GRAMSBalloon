#!/usr/bin/env ruby

require 'anlnext'
require 'GRAMSBalloon'

class MyApp < ANL::ANLApp
    def setup()
        chain GRAMSBalloon::SPIManager
        chain GRAMSBalloon::GetEnvironmentalData
        with_parameters(chip_select: 21)
        chain GRAMSBalloon::GetEnvironmentalData, "GetBME680Data2"
        with_parameters(chip_select: 19)
    end
end

a = MyApp.new
a.run(100, 1)
