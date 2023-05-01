#!/usr/bin/env ruby

require 'anlnext'
require 'GRAMSBalloon'

class MyApp < ANL::ANLApp
    def setup()
        chain GRAMSBalloon::SPIManager, "SPIManager_slowADC"
        with_parameters(spi_flags: (1<<5) + (1<<6) + (1<<7) + 3, baudrate: 1000000)
        chain GRAMSBalloon::GetSlowADCData
        with_parameters(chip_select: 17, SPI_manager_name: "SPIManager_slowADC", Va: 5.026,
        channels: [0, 1, 2, 3, 4], num_trials: 2)
    end
end

a = MyApp.new
a.run(10, 1)
