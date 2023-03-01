#!/usr/bin/env ruby

require 'anlnext'
require 'GRAMSBalloon'

class MyApp < ANL::ANLApp
    def setup()
        chain GRAMSBalloon::SPIManager
        chain GRAMSBalloon::GetBME680Data
    end
end

a = MyApp.new
a.run(100, 1)
