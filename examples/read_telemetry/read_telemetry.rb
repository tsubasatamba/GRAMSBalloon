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
    with_parameters(filenames: @filenames, sleep_ms: 1000, chatter: 0)
    chain GRAMSBalloon::InterpretTelemetry
    with_parameters(save_telemetry: false, receiver_module_name: "ReadTelemetry", run_ID_filename: "/Users/grams/settings/run_id/run_id_read.txt", chatter: 0)
    chain GRAMSBalloon::PushToMongoDB
  end
  attr_accessor :serial_path, :filenames
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
#a.filenames = Dir.glob("/Users/grams/data/telemetry/telemetry_000120_20230609124507_HK_*.dat")
a.filenames = Dir.glob("/Users/grams/data/telemetry/telemetry_000160_20230620151216_WF_*.dat")
a.run(:all, 1)
