#!/usr/bin/env ruby
require 'anlnext'
require 'GRAMSBalloon'

class MyApp < ANL::ANLApp
    def setup()

        chain GRAMSBalloon::ReceiveCommand
        with_parameters(serial_path: "/dev/ttyAMA1")


        #chain GRAMSBalloon::SPIManager, "SPIManager_RTD"
        #with_parameters(spi_flags: (1<<5) + (1<<6) + (1<<7) + 3, baudrate: 1000000)

        #chain GRAMSBalloon::SPIManager, "SPIManager_Env"
        #with_parameters(spi_flags: (1<<5) + (1<<6) + (1<<7) ,baudrate: 1000000)

        #chain GRAMSBalloon::SPIManager, "SPIManager_slowADC"
        #with_parameters(spi_flags: (1<<5) + (1<<6) + (1<<7) + 3, baudrate: 1000000)

        chain GRAMSBalloon::SPIManager
        with_parameters(spi_flags: (1<<5) + (1<<6) + (1<<7) + 3, baudrate: 1000000)

        rtd_cs_array = [7, 8, 16, 20, 21]
        rtd_cs_array.each_with_index do |cs, i|
          chain GRAMSBalloon::MeasureTemperatureWithRTDSensor, "MeasureTemperatureWithRTDSensor_#{i+1}"
          #with_parameters(chip_select: cs, SPI_manager_name: "SPIManager_RTD")
          with_parameters(chip_select: cs)
        end

        chain GRAMSBalloon::GetSlowADCData
        #with_parameters(chip_select: 17, SPI_manager_name: "SPIManager_slowADC", Va: 5.026,
        #channels: [0, 1, 2, 3], num_trials: 100)
        with_parameters(chip_select: 17, Va: 5.026,
        channels: [0,1,2], num_trials: 100)

        env_cs_array = [22, 5, 6, 19, 26]
        env_cs_array.each_with_index do |cs, i|
          chain GRAMSBalloon::GetEnvironmentalData,"GetEnvironmentalData_#{i+1}"
          #with_parameters(chip_select: cs, SPI_manager_name: "SPIManager_Env")
          with_parameters(chip_select: cs)
        end

        chain GRAMSBalloon::MeasureAcceleration

        #chain GRAMSBalloon::GetSlowADCData
        #with_parameters(chip_select: 17, SPI_manager_name: "SPIManager_slowADC", Va: 5.026,
        #channels: [0, 1, 2, 3], num_trials: 100)
        #with_parameters(chip_select: 17, Va: 5.026,
        #channels: [0,1,2,3], num_trials: 2)

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


a = MyApp.new

a.run(10000, 1)
