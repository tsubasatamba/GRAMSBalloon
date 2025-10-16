#!/usr/bin/env ruby
require 'anlnext'
require 'GRAMSBalloon'

class MyApp < ANL::ANLApp
    def setup()
        chain GRAMSBalloon::TelemMosquittoManager, "TelemMosquittoManager"
        with_parameters(host: ENV["PGRAMS_MOSQUITTO_HOST"], port: ENV["PGRAMS_MOSQUITTO_PORT"].to_i, password: ENV["PGRAMS_MOSQUITTO_PASSWD"], user: ENV["PGRAMS_MOSQUITTO_USER"], keep_alive: 60, chatter: 0, threaded_set: true, device_id: "HubComputer") do |m|
        m.set_singleton(0)
        end
        chain GRAMSBalloon::SendTelemetry
        with_parameters(
            topics: ["telemetry"],
            qos:0,
            save_telemetry: true,
            binary_filename_base: "telemetry_test",
            num_telem_per_file: 1000,
            chatter: 0,
        )
        chain GRAMSBalloon::Sleep
        with_parameters(sleep_sec: 1)
        chain GRAMSBalloon::RunIDManager
    end
end

a = MyApp.new
a.run(:all, 1)
