#include <string>
#include <fstream>
#include <iostream>
#include <sys/time.h>
#include <vector>
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "GRAMSBalloon/TelemetryDefinition.hh"

#define VERBOSE 2

#define MTR_NUM_ENV_SENSOR 5
#define MTR_NUM_CHAMBER_TEMP 3

using namespace gramsballoon;

double RTD_conversion(int adcValue)
{
    double rRef_ = 430.0;
    double res = (static_cast<double>(adcValue) * static_cast<double>(rRef_) / 400.0) / 32.0 - 256.0;
    return res;
}
double slow_ADC_conversion(int adcValue)
{
    double va = 5.026;
    return static_cast<double>(adcValue) / 4096 * va;
}

double pressure_conversion(int adcValue)
{
    double resistor = 100.0;
    double current = slow_ADC_conversion(adcValue) / resistor * 1000.; // mA
    return (current - 4.0) * 2.0 / 16.0;
}

double main_current_conversion(int adcValue)
{
    return (slow_ADC_conversion(adcValue) - 1.0) * 1.25;
}

int main(int argc, char **argv)
{
    constexpr int TELEMETRY_LENGTH = 132;

    if (argc != 2)
    {
        std::cerr << "Usage: ./make_telemetry_root inputfile outputfile" << std::endl;
        return -1;
    }
    const std::string input_filename(argv[1]);
    const std::string output_filename(argv[2]);
    std::ifstream ifs(input_filename, std::ios::in);

#if VERBOSE > 0
    std::cout << "Input filename: " << input_filename << std::endl;
    std::cout << "Output filename: " << output_filename << std::endl;
#endif

    std::vector<std::string>
        binary_files;

    while (!ifs.eof())
    {
        std::string line;
        std::getline(ifs, line);
        binary_files.push_back(line);
#if VERBOSE > 1
        std::cout << line << std::endl;
#endif
    }

    ifs.close();

    std::vector<std::vector<uint8_t>> telemetry;
    for (int i = 0; i < static_cast<int>(binary_files.size()); i++)
    {
        std::ifstream ifs_binary(binary_files[i]);

        while (!ifs_binary.eof())
        {
            std::vector<uint8_t> buf(TELEMETRY_LENGTH);
            for (int j = 0; j < TELEMETRY_LENGTH; j++)
            {
                if (ifs_binary.eof())
                {
                    break;
                }
                ifs_binary >> buf[i];
            }
            telemetry.push_back(buf);
        }
    }

    TFile *rootfile = TFile::Open(output_filename.c_str());
    TTree *HK_telemetry = new TTree("hk_telemetry", "hk_telemetry");
    TelemetryDefinition teldef;
    uint16_t start_code = 0xeb90;
    uint16_t telemetry_type = 0;
    timeval time_now;
    uint32_t telemetry_index = 0;
    int32_t run_id = -1;
    // footer
    uint16_t crc = 0;
    uint16_t stop_code = 0xc5c5;

    // HK
    uint32_t event_count = 0;
    // uint32_t trigger_count = 0;
    double chamber_pressure = 0.;
    std::vector<double> chamber_temperature(MTR_NUM_CHAMBER_TEMP);
    double valve_temperature = 0.;
    double outer_temperature = 0.;
    double TPC_HV_setting = 0.0;
    double TPC_HV_measure = 0.;
    double PMT_HV_setting = 0.0;
    double PMT_HV_measure = 0.;
    double CPU_temperature = 0.0;
    std::vector<double> env_temperature(MTR_NUM_ENV_SENSOR);
    std::vector<double> env_humidity(MTR_NUM_ENV_SENSOR);
    std::vector<double> env_pressure(MTR_NUM_ENV_SENSOR);
    std::vector<float> acceleration(3);
    std::vector<float> gyro(3);
    std::vector<float> magnet(3);
    float accel_sensor_temperature = 0.0;
    double main_current = 0.;
    double main_voltage = 0.;
    uint16_t last_command_index = 0;
    uint16_t last_command_code = 0;
    uint16_t command_reject_count = 0;
    uint64_t software_error_code = 0;
    HK_telemetry->Branch("start_code", &start_code, "start_code/s");
    HK_telemetry->Branch("telemetry_type", &telemetry_type, "telemetry_type/s");
    HK_telemetry->Branch("time_sec", &time_now.tv_sec, "time_sec/I");
    HK_telemetry->Branch("time_usec", &time_now.tv_usec, "time_usec/I");
    HK_telemetry->Branch("telemetry_index", &telemetry_index, "telemetry_index/i");
    HK_telemetry->Branch("run_id", &run_id, "run_id/I");
    HK_telemetry->Branch("CRC", &crc, "CRC/s");
    HK_telemetry->Branch("stop_code", &stop_code, "stop_code/s");
    HK_telemetry->Branch("event_count", &event_count, "event_count/i");
    HK_telemetry->Branch("chamber_pressure", &chamber_pressure, "chamber_pressure/D");
    for (int i = 0; i < MTR_NUM_CHAMBER_TEMP; i++)
    {
        HK_telemetry->Branch(Form("chamber_temperature_%d", i), chamber_temperature.data() + i, Form("chamber_temperature_%d/D", i));
    }
    HK_telemetry->Branch("outer_temperature", &outer_temperature, "outer_temperature/D");
    HK_telemetry->Branch("valve_temperature", &valve_temperature, "valve_temperature/D");
    HK_telemetry->Branch("TPC_HV_setting", &TPC_HV_setting, "TPC_HV_setting/D");
    HK_telemetry->Branch("TPC_HV_measure", &TPC_HV_measure, "TPC_HV_measure/D");
    HK_telemetry->Branch("PMT_HV_setting", &PMT_HV_setting, "PMT_HV_setting/D");
    HK_telemetry->Branch("PMT_HV_measure", &PMT_HV_measure, "PMT_HV_measure/D");
    HK_telemetry->Branch("CPU_temperature", &CPU_temperature, "CPU_temperature/D");

    for (int i = 0; i < MTR_NUM_ENV_SENSOR; i++)
    {
        HK_telemetry->Branch(Form("env_temperature_%d", i), env_temperature.data() + i, Form("env_temperature_%d/D", i));
        HK_telemetry->Branch(Form("env_humidity_%d", i), env_humidity.data() + i, Form("env_humidity_%d/D", i));
        HK_telemetry->Branch(Form("env_pressure_%d", i), env_pressure.data() + i, Form("env_pressure_%d/D",i));
    }

    HK_telemetry->Branch("acceleration_x", &acceleration[0], "acceleration_x/F");
    HK_telemetry->Branch("acceleration_y", &acceleration[1], "acceleration_y/F");
    HK_telemetry->Branch("acceleration_z", &acceleration[2], "acceleration_z/F");

    HK_telemetry->Branch("gyro_x", &gyro[0], "gyro_x/F");
    HK_telemetry->Branch("gyro_y", &gyro[1], "gyro_y/F");
    HK_telemetry->Branch("gyro_z", &gyro[2], "gyro_z/F");

    HK_telemetry->Branch("magnet_x", &magnet[0], "magnet_x/F");
    HK_telemetry->Branch("magnet_y", &magnet[1], "magnet_y/F");
    HK_telemetry->Branch("magnet_z", &magnet[2], "magnet_z/F");

    HK_telemetry->Branch("accel_sensor_temperature", &accel_sensor_temperature, "accel_sensor_temperature/F");

    HK_telemetry->Branch("main_current", &main_current, "main_current/D");
    HK_telemetry->Branch("main_voltage", &main_voltage, "main_voltage/D");
    HK_telemetry->Branch("last_command_code", &last_command_code, "last_command_code/s");
    HK_telemetry->Branch("last_command_index", &last_command_index, "last_command_index/s");
    HK_telemetry->Branch("command_reject_count", &command_reject_count, "command_reject_count/s");
    HK_telemetry->Branch("software_command_error_code", &software_error_code, "software_error_code/l");

    for (int i = 0; i < static_cast<int>(telemetry.size()); i++)
    {

        teldef.setTelemetry(telemetry[i]);
        teldef.interpret();
        start_code = teldef.StartCode();
        telemetry_type = teldef.TelemetryType();
        time_now = teldef.TimeNow();
        telemetry_index = teldef.TelemetryIndex();
        run_id = teldef.RunID();
        crc = teldef.CRC();
        stop_code = teldef.StopCode();
        event_count = teldef.EventCount();
        // trigger_count = teldef.TriggerCount();
        chamber_pressure = pressure_conversion(teldef.ChamberPressure());

        for (int j = 0; j < static_cast<int>(MTR_NUM_CHAMBER_TEMP); j++)
        {
            chamber_temperature[j] = RTD_conversion(teldef.ChamberTemperature(j));
        }

        valve_temperature = RTD_conversion(teldef.ValveTemperature());
        outer_temperature = RTD_conversion(teldef.OuterTemperature());
        TPC_HV_setting = teldef.TPCHVSetting();
        TPC_HV_measure = slow_ADC_conversion(teldef.TPCHVMeasure());
        PMT_HV_setting = teldef.PMTHVSetting();
        // tpc_hv_measure_current
        CPU_temperature = teldef.CPUTemperature();

        for (int j = 0; j < MTR_NUM_ENV_SENSOR; j++)
        {
            env_temperature[j] = teldef.EnvTemperature(j);
            env_humidity[j] = teldef.EnvHumidity(j);
            env_pressure[j] = teldef.EnvPressure(j);
        }

        for (int j = 0; j < 3; j++)
        {
            acceleration[j] = teldef.Acceleration(j);
            gyro[j] = teldef.Gyro(j);
            magnet[j] = teldef.Magnet(j);
        }

        accel_sensor_temperature = teldef.AccelSensorTemperature();
        main_current = main_current_conversion(teldef.MainCurrent());
        main_voltage = slow_ADC_conversion(teldef.MainVoltage());
        last_command_index = teldef.LastCommandIndex();
        last_command_code = teldef.LastCommandCode();
        command_reject_count = teldef.CommandRejectCount();
        software_error_code = teldef.SoftwareErrorCode();

        HK_telemetry->Fill();
    }
    rootfile->cd();
    HK_telemetry->Write();
    rootfile->Clone();

    return 0;
}
