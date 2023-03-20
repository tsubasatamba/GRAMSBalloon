#!/usr/bin/env ruby

require 'anlnext'
require 'GRAMSBalloon'

class MyApp < ANL::ANLApp
    def setup()
        chain GRAMSBalloon::SPIManager
        with_parameters(spi_flags: (1<<5) + (1<<6) + (1<<7) + 1)
        chain GRAMSBalloon::MeasureTemperatureWithRTDSensor
        with_parameters(chip_select: 8)
    end
end

a = MyApp.new
a.run(100, 1)
