#!/usr/bin/env ruby
require 'anlnext'
require 'GRAMSBalloon'
require 'inifile'

class MyApp < ANL::ANLApp
  attr_accessor :inifile
  def setup()
    @inifile = IniFile.load('./network.cfg')
    if inifile.nil?
      puts "Error: network.cfg not found"
      exit 1
    end
    chain GRAMSBalloon::MosquittoManager
    with_parameters(host: ENV["PGRAMS_MOSQUITTO_HOST"], port: ENV["PGRAMS_MOSQUITTO_PORT"].to_i, password: ENV["PGRAMS_MOSQUITTO_PASSWD"], user: ENV["PGRAMS_MOSQUITTO_USER"], keep_alive: 60, chatter: 0, threaded_set: true, device_id: "hubcomputer") do |m|
      m.set_singleton(0)
    end
    chain GRAMSBalloon::ReceiveCommand
    with_parameters(topic: "command", chatter: 100, qos: 0, binary_filename_base: ENV["HOME"]+"/command_test/command") do |m|
      m.set_singleton(0)
    end
    chain GRAMSBalloon::SocketCommunicationManager, "SocketCommunicationManager_TPC"
    with_parameters(ip: @inifile["TPC"]["ip"], port: @inifile["TPC"]["comport"].to_i, timeout: 1000.0, chatter: 100) do |m|
      m.set_singleton(0)
    end
    chain GRAMSBalloon::SocketCommunicationManager, "SocketCommunicationManager_TPC_rsv"
    with_parameters(ip: @inifile["TPC"]["ip"], port: @inifile["TPC"]["telport"].to_i, timeout: 1000.0, chatter: 100) do |m|
      m.set_singleton(0)
    end
    chain GRAMSBalloon::DistributeCommand
    with_parameters(topic: @inifile["TPC"]["comtopic"], chatter: 100, SocketCommunicationManager_name: "SocketCommunicationManager_TPC") do |m|
      m.set_singleton(0)
    end
    chain GRAMSBalloon::ReceiveStatusFromDAQComputer
    with_parameters(SocketCommunicationManager_name:"SocketCommunicationManager_TPC_rsv", chatter:1)
    chain GRAMSBalloon::DividePacket
    with_parameters(ReceiveStatusFromDAQComputer_name: "ReceiveStatusFromDAQComputer", chatter: 4)
    chain GRAMSBalloon::PassTelemetry
    with_parameters(DividePacket_name: "DividePacket", topic: @inifile["TPC"]["teltopic"], chatter: 100)
    #chain GRAMSBalloon::Sleep
    #with_parameters(sleep_sec: 1)
    
    chain GRAMSBalloon::RunIDManager
    with_parameters(
      filename: ENV["HOME"] + "/settings/run_id/run_id.txt"
    ) do |m|
        m.set_singleton(0)
    end
  end
end

a = MyApp.new


a.num_parallels = 1
a.run(1000000000, 1)
exit_status = 1
puts "exit_status: #{exit_status}"
exit exit_status

