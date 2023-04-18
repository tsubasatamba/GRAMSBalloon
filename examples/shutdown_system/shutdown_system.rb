#!/usr/bin/env ruby

require 'anlnext'
require 'GRAMSBalloon'

class MyApp < ANL::ANLApp
    def setup()
        chain GRAMSBalloon::ShutdownSystem
    end
end

a = MyApp.new
a.run(1000, 1)
