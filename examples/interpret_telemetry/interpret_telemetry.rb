#!/usr/bin/env ruby

require 'anlnext'
require 'GRAMSBalloon'
require 'inifile'

class MyApp < ANL::ANLApp
  attr_accessor :inifile
  def setup()
    @inifile = IniFile.load('../../settings/network.cfg')
    if inifile.nil?
      puts "Error: network.cfg not found"
      exit 1
    end
    chain GRAMSBalloon::TelemMosquittoManager, "TelemMosquittoManager"
    with_parameters(host: ENV["PGRAMS_MOSQUITTO_HOST"], port: ENV["PGRAMS_MOSQUITTO_PORT"].to_i, password: ENV["PGRAMS_MOSQUITTO_PASSWD"], user: ENV["PGRAMS_MOSQUITTO_USER"], keep_alive: 60, chatter: 0, threaded_set: true, device_id: "Ground") do |m|
      m.set_singleton(0)
    end
    subsystems = ["Orchestrator"]
    for subsystem in subsystems
    chain GRAMSBalloon::ReceiveTelemetry, "ReceiveTelemetry_#{subsystem}"
    with_parameters(topic: @inifile[subsystem]["iridiumteltopic"], chatter: 2)
    chain GRAMSBalloon::InterpretBaseTelemetry, "InterpretDAQFormattedTelemetry_#{subsystem}"
    with_parameters(receiver_module_name: "ReceiveTelemetry_#{subsystem}", chatter: 2)
  end
  #chain GRAMSBalloon::ReceiveTelemetry, "ReceiveTelemetry_HK"
  #with_parameters(topic: "hub_telemetry", chatter: 0)
  #chain GRAMSBalloon::InterpretHKTelemetry, "InterpretHKTelemetry"
  #with_parameters(receiver_module_name: "ReceiveTelemetry_HK", save_telemetry: true, num_telem_per_file: 1000, chatter: 2)
  chain GRAMSBalloon::Sleep
  with_parameters(sleep_sec:1)
  end
end

a = MyApp.new
a.run(:all, 1)
