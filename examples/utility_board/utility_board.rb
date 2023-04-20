#!/usr/bin/env ruby
require 'anlnext'
require 'GRAMSBalloon'

class MyApp < ANL::ANLApp
    def setup()
        chain GRAMSBalloon::ReceiveCommand
        with_parameters(serial_path: "/dev/ttyAMA1")

        chain GRAMSBalloon::SPIManager,"SPI1"
        with_parameters(spi_flags: (1<<5) + (1<<6) + (1<<7) + 1)

        chian GRAMSBalloon::SPIManager,"SPI2"
        with_parameters(spi_flags: (1<<5) + (1<<6) + (1<<7))

        chain GRAMSBalloon::MeasureTemperatureWithRTDSensor,"RTD1"
        with_parameters(chip_select: 7,SPI_manager_name: "SPI1")

        chain GRAMSBalloon::MeasureTemperatureWithRTDSensor,"RTD2"
        with_parameters(chip_select: 8,SPI_manager_name: "SPI1")

        chain GRAMSBalloon::MeasureTemperatureWithRTDSensor,"RTD3"
        with_parameters(chip_select: 16,SPI_manager_name: "SPI1")

        chain GRAMSBalloon::MeasureTemperatureWithRTDSensor,"RTD4"
        with_parameters(chip_select: 20,SPI_manager_name: "SPI1")

        chain GRAMSBalloon::MeasureTemperatureWithRTDSensor,"RTD5"
        with_parameters(chip_select: 21,SPI_manager_name: "SPI1")

        chain GRAMSBalloon::GetEnvironmentalData,"GetBME680Data1"
        with_parameters(chip_select: 22,SPI_manager_name: "SPI2")

        chain GRAMSBalloon::GetEnvironmentalData, "GetBME680Data2"
        with_parameters(chip_select: 5,SPI_manager_name: "SPI2")

        chain GRAMSBalloon::GetEnvironmentalData,"GetBME680Data3"
        with_parameters(chip_select: 6,SPI_manager_name: "SPI2")

        chain GRAMSBalloon::GetEnvironmentalData, "GetBME680Data4"
        with_parameters(chip_select: 19,SPI_manager_name: "SPI2")

        chain GRAMSBalloon::GetEnvironmentalData, "GetBME680Data5"
        with_parameters(chip_select: 26,SPI_manager_name: "SPI2")

        chain GRAMSBalloon::MeasureAcceleration

        chain GRAMSBalloon::GetRaspiStatus

        chain GRAMSBalloon::AnalogDiscoveryManager

        chain GRAMSBalloon::ReadWaveform
        with_parameters(
            trig_device: 0,
            trig_channel: 0,
            trig_mode: 2,
            trig_level: 0.1,
            trig_position: 0.0,
            time_window: 90.0, # us
            sample_frequency: 2.0, #MHz
            output_filename_base: "DAQ_output",
            num_events_per_file: 5
        )

        chain GRAMSBalloon::SendTelemetry
        with_parameters(
            serial_path: "/dev/ttyAMA0"
        )
    end
end

a = MyApp.new
a.run(:all, 1)