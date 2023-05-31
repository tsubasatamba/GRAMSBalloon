#!/usr/bin/env ruby

require 'anlnext'
require 'GRAMSBalloon'
MHz = 1000000
class MyApp < ANL::ANLApp
  def setup()
    chain GRAMSBalloon::SPIManager, "SPIManager_slowADC"
    with_parameters(spi_flags: (1<<5) + (1<<6) + (1<<7) + 3, baudrate: MHz*1)
    chain GRAMSBalloon::GetSlowADCData
    with_parameters(chip_select: 17, SPI_manager_name: "SPIManager_slowADC", Va: 5.026,
                    channels: [0,1,2], num_trials: 100, chatter: 1)
  end
end

a = MyApp.new
a.run(100, 1)
