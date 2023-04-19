#!/usr/bin/env ruby

require 'anlnext'
require 'GRAMSBalloon'
require 'HSQuickLook'

class MyApp < ANL::ANLApp
  def setup()
    chain HSQuickLook::MongoDBClient
    #with_parameters(host: @host_name, database: @database_name, instantiation: false)
    chain GRAMSBalloon::ReceiveTelemetry
    with_parameters(serial_path: "/dev/tty.usbserial-14210")
    chain GRAMSBalloon::InterpretHKTelemetry
  end
  attr_accessor :host_name, :database_name
end

a = MyApp.new
a.run(100, 1)
