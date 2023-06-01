#!/usr/bin/env ruby

require 'anlnext'
require 'GRAMSBalloon'
require 'HSQuickLook'

class MyApp < ANL::ANLApp
  def setup()
    chain HSQuickLook::MongoDBClient
    with_parameters(database: "grams")
    #with_parameters(host: @host_name, database: @database_name, instantiation: false)
    chain GRAMSBalloon::ReceiveTelemetry
    with_parameters(serial_path: "/dev/tty.usbserial-1430", open_mode: 2, chatter: 1)
    chain GRAMSBalloon::InterpretTelemetry
    with_parameters(save_telemetry: true, num_telem_per_file: 1000, chatter: 1, binary_filename_base: "/Users/grams/data/telemetry/telemetry")
    chain GRAMSBalloon::PushToMongoDB
  end
  attr_accessor :host_name, :database_name
end

a = MyApp.new
a.run(:all, 1)
