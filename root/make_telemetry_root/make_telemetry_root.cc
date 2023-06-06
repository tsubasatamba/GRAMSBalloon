#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TelemetryDefinition.hh"

#define VERBOSE 2

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
    return adcValue / 4096 * va;
}

int main(int argc, char **argv)
{
    constexpr int TELEMETRY_LENGTH = 132;

    if (argc != 2)
    {
        std::cerr << "Usage: ./Prod inputfile outputfile" << std::endl;
        return -1;
    }
    const std::string input_filename(argv[1]);
    const std::string output_filename(argv[2]);
    std::fstream ifs(input_filename, std::ios_base::binary | std::ios::in);

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
    for (int i = 0; i < binary_files.size(); i++)
    {
        std::fstream ifs(binary_files[i]);

        while (!ifs.eof())
        {
            std::vector<uint8_t> buf(TELEMETRY_LENGTH);
            for (int j = 0; j < TELEMETRY_LENGTH; j++)
            {
                ifs >> buf[i];
            }
            telemetry.push_back(buf);
        }
    }

    TFile *rootfile = TFile::Open(output_filename.c_str());
    std::unique_ptr<TTree> HK_telemetry = std::make_unique<TTree>("HK", "HK");

    for (int i = 0; i < telemetry.size(); i++)
    {
        TelemetryDefinition teldef;
        teldef.setTelemetry(telemetry[i]);
        teldef.interpret();
        uint16_t start_code = teldef.StartCode();
        uint16_t telemetry_type = teldef.TelemetryType();
        timeval time = teldef.TimeNow();
        uint32_t telemetry_index = teldef.TelemetryIndex();
        int32_t run_id = teldef.RunID();
        uint32_t crc = teldef.CRC();
        uint16_t stop_code = teldef.StopCode();
        uint32_t event_count = teldef.EventCount();
        uint32_t trigger_count = teldef.TriggerCount();
        uint16_t chamber_pressure = teldef.ChamberPressure();
        std::vector<double> chamber_temperature(3);

        for (int i = 0; i < chamber_temperature.size(); i++)
        {
            chamber_temperature[i] = RTD_conversion(teldef.ChamberTemperature(i));
        }

        double valve_temperature = RTD_conversion(teldef.ValveTemperature());
        double outer_temperature = RTD_conversion(teldef.OuterTemperature());
        double tpc_hv_setting = teldef.TPCHVSetting();
        double tpc_hv_measure = slow_ADC_conversion(teldef.TPCHVMeasure());
        double pmt_hv_setting = teldef.PMTHVSetting();
        // tpc_hv_measure_current


        HK_telemetry->Branch("start_code", &start_code, "start_code/I");
        HK_telemetry->Branch("telemetry_type", &telemetry_type, "telemetry_type/I");
        HK_telemetry->Branch("time_sec", &time.tv_sec, "time_sec/I");
        HK_telemetry->Branch("time_usec", &time.tv_usec, "time_usec/I");
        HK_telemetry->Branch("telemetry_index", &telemetry_index, "telemetry_index/I");
        HK_telemetry->Branch("run_id", &run_id, "run_id/I");
        HK_telemetry->Branch("CRC", &crc, "CRC/I");
        HK_telemetry->Branch("stop_code", &stop_code, "stop_code/I");
        HK_telemetry->Branch("event_count", &event_count, "event_count/I");

        HK_telemetry->Fill();
    }
}
