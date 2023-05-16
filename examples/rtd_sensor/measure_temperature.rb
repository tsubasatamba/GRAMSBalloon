#!/usr/bin/env ruby

require 'anlnext'
require 'GRAMSBalloon'

class MyApp < ANL::ANLApp
    def setup()
        chain GRAMSBalloon::SPIManager
        with_parameters(spi_flags: (1<<5) + (1<<6) + (1<<7) + 1)
        #chain GRAMSBalloon::MeasureTemperatureWithRTDSensor
        #with_parameters(chip_select: 8)
        rtd_cs_array = [7, 8, 16, 20, 21]
        rtd_cs_array.each_with_index do |cs, i|
          chain GRAMSBalloon::MeasureTemperatureWithRTDSensor, "MeasureTemperatureWithRTDSensor_#{i+1}"
          #with_parameters(chip_select: cs, SPI_manager_name: "SPIManager_RTD")
          with_parameters(chip_select: cs)
        end
    end
end

a = MyApp.new
a.run(100, 1)
