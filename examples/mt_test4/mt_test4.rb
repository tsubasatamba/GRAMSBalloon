#!/usr/bin/env ruby
require 'anlnext'
require 'GRAMSBalloon'

class MyApp < ANL::ANLApp
    def setup()

        chain GRAMSBalloon::ReceiveCommand
        with_parameters(serial_path: "/dev/ttyAMA1") do |m|
            m.set_singleton(2)
        end

        chain GRAMSBalloon::SPIManager, "SPIManager_RTD"
        with_parameters(spi_flags: (1<<5) + (1<<6) + (1<<7) + 1, baudrate: 1000000) do |m|
            m.set_singleton(0)
        end

        chain GRAMSBalloon::SPIManager, "SPIManager_Env"
        with_parameters(baudrate: 1000000) do |m|
            m.set_singleton(0)
        end

        chain GRAMSBalloon::SPIManager, "SPIManager_slowADC"
        with_parameters(spi_flags: (1<<5) + (1<<6) + (1<<7) + 3, baudrate: 1000000) do |m|
            m.set_singleton(0)
        end

        rtd_cs_array = [7, 8, 16, 20, 21]
        rtd_cs_array.each_with_index do |cs, i|
          chain GRAMSBalloon::MeasureTemperatureWithRTDSensor, "MeasureTemperatureWithRTDSensor_#{i+1}"
          with_parameters(chip_select: cs, SPI_manager_name: "SPIManager_RTD") do |m|
              m.set_singleton(0)
          end
        end

        env_cs_array = [22, 5, 6, 19, 26]
        env_cs_array.each_with_index do |cs, i|
          chain GRAMSBalloon::GetEnvironmentalData,"GetEnvironmentalData_#{i+1}"
          with_parameters(chip_select: cs, SPI_manager_name: "SPIManager_Env") do |m|
              m.set_singleton(0)
          end
        end

        chain GRAMSBalloon::MeasureAcceleration
        with_parameters do |m|
            m.set_singleton(0)
        end

        chain GRAMSBalloon::GetSlowADCData
        with_parameters(chip_select: 17, SPI_manager_name: "SPIManager_slowADC", Va: 5.026,
        channels: [0, 1, 2, 3], num_trials: 100) do |m|
            m.set_singleton(0)
        end

        chain GRAMSBalloon::GetRaspiStatus do |m|
            m.set_singleton(0)
        end

        chain GRAMSBalloon::ShutdownSystem do |m|
            m.set_singleton(0)
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
          PMTHVController_module_name: "ControlHighVoltage_PMT"
        ) do |m|
            m.set_singleton(0)
        end

    end
end

main_modules = ["SPIManager_RTD", "SPIManager_Env", "SPIManager_slowADC"]
for i in 1..5 do
  main_modules << "MeasureTemperatureWithRTDSensor_#{i}"
end

for i in 1..5 do
  main_modules << "GetEnvironmentalData_#{i}"
end

main_modules << "MeasureAcceleration" << "GetSlowADCData" << "GetRaspiStatus"
main_modules << "SendTelemetry"

daq_modules = []
command_modules = ["ReceiveCommand"]

#main_modules = ["SPIManager_Env", "SPIManager_slowADC", "GetSlowADCData"]
#daq_modules = []
#command_modules = []
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

a.run(10000, 1)
