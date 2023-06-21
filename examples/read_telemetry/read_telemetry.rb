#!/usr/bin/env ruby

require 'anlnext'
require 'GRAMSBalloon'
require 'HSQuickLook'

class MyApp < ANL::ANLApp
  def setup()
    chain HSQuickLook::MongoDBClient
    with_parameters(database: "grams")
    #with_parameters(host: @host_name, database: @database_name, instantiation: false)
    chain GRAMSBalloon::ReadTelemetry
    with_parameters(filenames: ["telemetry_000159_20230614151309_HK_000000.dat"], sleep_ms: 500, chatter: 0)
    chain GRAMSBalloon::InterpretTelemetry
    with_parameters(save_telemetry: false, receiver_module_name: "ReadTelemetry", chatter: 0)
    chain GRAMSBalloon::PushToMongoDB
  end
  attr_accessor :serial_path
end

def get_serial_path()
  filename = Dir.home + "/settings/serial/telemetry_port.txt"
  ret = ""
  File.open(filename, mode="rt") do |f|
    f.each_line do |line|
      ret = line.strip
    end
  end
  return ret
end

a = MyApp.new
a.serial_path = get_serial_path()

a.run(:all, 1)
