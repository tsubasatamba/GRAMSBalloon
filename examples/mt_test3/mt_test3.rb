#!/usr/bin/env ruby
require 'anlnext'
require 'GRAMSBalloon'

class MyApp < ANL::ANLApp
    def setup()
        chain GRAMSBalloon::ReceiveCommand
        with_parameters(serial_path: "/dev/ttyAMA0")

        chain GRAMSBalloon::SPIManager, "SPIManager_RTD"
        with_parameters(spi_flags: (1<<5) + (1<<6) + (1<<7) + 1)

        chain GRAMSBalloon::SPIManager, "SPIManager_slowADC"
        with_parameters(spi_flags: (1<<5) + (1<<6) + (1<<7) + 3)

        chain GRAMSBalloon::SPIManager, "SPIManager_Env"

        rtd_cs_array = [7, 8, 16, 20, 21]
        rtd_cs_array.each_with_index do |cs, i|
          chain GRAMSBalloon::MeasureTemperatureWithRTDSensor, "MeasureTemperatureWithRTDSensor_#{i+1}"
          with_parameters(chip_select: cs, SPI_manager_name: "SPIManager_RTD")
        end

        env_cs_array = [22, 5, 6, 19, 26]
        env_cs_array.each_with_index do |cs, i|
          chain GRAMSBalloon::GetEnvironmentalData,"GetEnvironmentalData_#{i+1}"
          with_parameters(chip_select: cs, SPI_manager_name: "SPIManager_Env")
        end

        chain GRAMSBalloon::MeasureAcceleration

        chain GRAMSBalloon::GetSlowADCData
        with_parameters(chip_select: 17, SPI_manager_name: "SPIManager_slowADC", Va: 5.026,
        channels: [0, 1], num_trials: 100)

        chain GRAMSBalloon::GetRaspiStatus

        chain GRAMSBalloon::AnalogDiscoveryManager

        chain GRAMSBalloon::ControlHighVoltage, "ControlHighVoltage_TPC"
        
        chain GRAMSBalloon::ControlHighVoltage, "ControlHighVoltage_PMT"

        chain GRAMSBalloon::ShutdownSystem
        
        chain GRAMSBalloon::ReadWaveform
        with_parameters(
            trig_device: 0,
            trig_channel: 0,
            trig_mode: 2,
            trig_level: 0.1,
            trig_position: 0.0,
            time_window: 1000.0, # us
            sample_frequency: 2.0, #MHz
            output_filename_base: "DAQ_output",
            num_events_per_file: 10,
            start_reading: true
        )

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
          PMTHVController_module_name: "ControlHighVoltage_PMT"
        )
    end
end

main_modules = ["SPIManager_RTD", "SPIManager_Env", "SPIManager_slowADC"]
for i in 1..5 do
  main_modules << "MeasureTemperatureWithRTDSensor_#{i}"
end

for i in 1..5 do
  main_modules << "GetEnvironmentalData_#{i}"
end

main_modules << "MeasureAcceleration" << "GetSlowADCData" << "GetRaspiStatus" << "AnalogDiscoveryManager"
main_modules << "ControlHighVoltage_TPC" << "ControlHighVoltage_PMT" << "SendTelemetry"

daq_modules = ["ReadWaveform"]
command_modules = ["ReceiveCommand"]

a = MyApp.new


a.num_parallels = 3
a.modify do |m|
  main_modules.each do |mod|
    m.get_parallel_module(1, mod).off
    m.get_parallel_module(2, mod).off
  end
  daq_modules.each do |mod|
    m.get_parallel_module(0, mod).off
    m.get_parallel_module(2, mod).off
  end
  command_modules.each do |mod|
    m.get_parallel_module(0, mod).off
    m.get_parallel_module(1, mod).off
  end
end

a.run(100, 1)
