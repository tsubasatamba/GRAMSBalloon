#!/usr/bin/env ruby
require 'anlnext'
require 'GRAMSBalloon'

class MyApp < ANL::ANLApp
    def setup()

        chain GRAMSBalloon::ReceiveCommand
        with_parameters(serial_path: "/dev/ttyAMA1", chatter: 1) do |m|
            m.set_singleton(1)
        end

        chain GRAMSBalloon::SPIManager
        with_parameters(spi_flags: (1<<5) + (1<<6) + (1<<7) + 3, baudrate: 1000000) do |m|
          m.set_singleton(0)
        end


        rtd_cs_array = [8, 7, 16, 20, 21]
        rtd_cs_array.each_with_index do |cs, i|
          chain GRAMSBalloon::MeasureTemperatureWithRTDSensor, "MeasureTemperatureWithRTDSensor_#{i+1}"
          with_parameters(chip_select: cs) do |m|
              m.set_singleton(0)
          end
        end

        env_cs_array = [26, 19, 6, 5, 22]
        env_cs_array.each_with_index do |cs, i|
          chain GRAMSBalloon::GetEnvironmentalData,"GetEnvironmentalData_#{i+1}"
          with_parameters(chip_select: cs) do |m|
              m.set_singleton(0)
          end
        end

        chain GRAMSBalloon::MeasureAcceleration
        with_parameters(calibrate_gyro: true) do |m|
            m.set_singleton(0)
        end

        chain GRAMSBalloon::GetSlowADCData
        with_parameters(chip_select: 17, Va: 5.026,
        channels: [0, 1, 2, 3, 4], num_trials: 100, chatter: 1) do |m|
            m.set_singleton(0)
        end

        chain GRAMSBalloon::GetRaspiStatus do |m|
            m.set_singleton(0)
        end

        chain GRAMSBalloon::AnalogDiscoveryManager do |m|
            m.set_singleton(0)
        end

        chain GRAMSBalloon::ControlHighVoltage, "ControlHighVoltage_TPC"
        with_parameters(device_id: 0, channel: 0, voltages: [], upper_limit_voltage: 0.1) do |m|
          m.set_singleton(0)
        end
        
        chain GRAMSBalloon::ControlHighVoltage, "ControlHighVoltage_PMT"
        with_parameters(device_id: 0, channel: 1, voltages: [], upper_limit_voltage: 0.1) do |m|
          m.set_singleton(0)
        end
                
        chain GRAMSBalloon::ReadWaveform
        with_parameters(
            trig_device: 0,
            trig_channel: 0,
            trig_mode: 2,
            trig_level: 0.1,
            trig_position: 0.0,
            time_window: 300.0, # us
            sample_frequency: 20.0, #MHz
            output_filename_base: "DAQ_output",
            num_events_per_file: 100,
            start_reading: false
        ) do |m|
          m.set_singleton(2)
        end


        chain GRAMSBalloon::SendTelemetry
        with_parameters(
          serial_path: "/dev/ttyAMA0",
          MeasureTemperature_module_names: ["MeasureTemperatureWithRTDSensor_1",
                                            "MeasureTemperatureWithRTDSensor_2",
                                            "MeasureTemperatureWithRTDSensor_3",
                                            "MeasureTemperatureWithRTDSensor_4",
                                            "MeasureTemperatureWithRTDSensor_5"],
          GetEnvironmentalData_module_names: ["GetEnvironmentalData_1",
                                              "GetEnvironmentalData_2",
                                              "GetEnvironmentalData_3",
                                              "GetEnvironmentalData_4",
                                              "GetEnvironmentalData_5"],
          TPCHVController_module_name: "ControlHighVoltage_TPC",
          PMTHVController_module_name: "ControlHighVoltage_PMT",
          chatter: 0
        ) do |m|
            m.set_singleton(0)
        end

        chain GRAMSBalloon::RunIDManager do |m|
            m.set_singleton(0)
        end

        chain GRAMSBalloon::ShutdownSystem do |m|
            m.set_singleton(0)
        end
    end
end




main_modules = ["SPIManager"]
for i in 1..5 do
  main_modules << "MeasureTemperatureWithRTDSensor_#{i}"
end

for i in 1..5 do
  main_modules << "GetEnvironmentalData_#{i}"
end

main_modules << "MeasureAcceleration" << "GetSlowADCData" << "GetRaspiStatus"
main_modules << "AnalogDiscoveryManager" << "ControlHighVoltage_TPC" << "ControlHighVoltage_PMT"
main_modules << "SendTelemetry" << "RunIDManager" << "ShutdownSystem"

daq_modules = ["ReadWaveform"]
command_modules = ["ReceiveCommand"]


a = MyApp.new


a.num_parallels = 3

a.modify do |m|
  main_modules.each do |mod|
    m.get_parallel_module(1, mod).off
    m.get_parallel_module(2, mod).off
  end
  command_modules.each do |mod|
    m.get_parallel_module(0, mod).off
    m.get_parallel_module(2, mod).off
  end
  daq_modules.each do |mod|
    m.get_parallel_module(0, mod).off
    m.get_parallel_module(1, mod).off
  end
end

a.run(10, 1)
