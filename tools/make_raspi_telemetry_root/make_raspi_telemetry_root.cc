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

double RTD_conversion(int adcValue) {
  const double rRef_ = 430.0;
  const double res = (static_cast<double>(adcValue) * static_cast<double>(rRef_) / 400.0) / 32.0 - 256.0;
  return res;
}
double slow_ADC_conversion(int adcValue) {
  const double va = 5.026;
  return static_cast<double>(adcValue) / 4096.0 * va;
}

double TPC_current_conversion(int adcValue) {
  const double va = 5.026;
  const double voltage = static_cast<double>(adcValue) / 4096.0 * va;
  const double current = voltage / 4.0 * 200.0;
  return current;
}

double pressure_conversion(int adcValue) {
  double resistor = 100.0;
  double current = slow_ADC_conversion(adcValue) / resistor * 1000.; // mA
  return (current - 4.0) * 2.0 / 16.0;
}

double main_current_conversion(int adcValue) {
  return (slow_ADC_conversion(adcValue) - 1.0) * 1.25;
}

double main_voltage_conversion(int adcValue) {
  return slow_ADC_conversion(adcValue) * 24.0 / 3.34;
}

int main(int argc, char** argv) {
  constexpr int TELEMETRY_LENGTH = 132;

  if (argc != 3) {
    std::cerr << "Usage: ./make_telemetry_root inputfile outputfile" << std::endl;
    return -1;
  }
  const std::string input_filename(argv[1]);
  const std::string output_filename(argv[2]);
  std::ifstream ifs_input(input_filename, std::ios::in);

  if (VERBOSE > 0) {
    std::cout << "Input filename: " << input_filename << std::endl;
    std::cout << "Output filename: " << output_filename << std::endl;
  }

  std::vector<std::string> binary_files;
  std::string filename_buffer;
  while (ifs_input >> filename_buffer) {
    binary_files.push_back(filename_buffer);
    if (VERBOSE > 1) {
      std::cout << filename_buffer << std::endl;
    }
  }

  ifs_input.close();

  std::vector<std::vector<uint8_t>> telemetry;
  for (int i = 0; i < static_cast<int>(binary_files.size()); i++) {
    std::ifstream ifs_binary(binary_files[i], std::ios::binary);
    if (!ifs_binary) {
      std::cerr << "File open error" << std::endl;
      exit(1);
    }
    while (!ifs_binary.eof()) {
      std::vector<uint8_t> buf(TELEMETRY_LENGTH);
      bool is_bad_data = false;
      for (int j = 0; j < TELEMETRY_LENGTH; j++) {
        if (ifs_binary.eof()) {
          break;
        }
        char c;
        ifs_binary.read(&c, 1);
        const uint8_t byte = static_cast<uint8_t>(c);
        if (j > 2 && j < TELEMETRY_LENGTH - 1 && byte == 0x79 && buf[j - 1] == 0xd2 && buf[j - 2] == 0xa4 && buf[j - 3] == 0xc5) {
          is_bad_data = true;
          break;
        }
        buf[j] = byte;
      }
      if (ifs_binary.eof()) break;
      if (is_bad_data) continue;
      telemetry.push_back(buf);
    }
    ifs_binary.close();
  }

  TFile* rootfile = TFile::Open(output_filename.c_str(), "recreate");
  TTree* tree = new TTree("hk_telemetry", "hk_telemetry");
  TelemetryDefinition teldef;

  // branches
  uint32_t start_code = 0;
  uint16_t telemetry_type = 0;
  timeval time_now;
  uint32_t telemetry_index = 0;
  int32_t run_id = -1;
  uint16_t crc = 0;
  uint32_t stop_code = 0;
  uint32_t event_count = 0;
  uint32_t current_event_id = 0;
  double chamber_pressure = 0.0;
  std::vector<double> chamber_temperature(MTR_NUM_CHAMBER_TEMP);
  double valve_temperature = 0.0;
  double outer_temperature = 0.0;
  double TPC_HV_setting = 0.0;
  double TPC_HV_measure = 0.0;
  double PMT_HV_setting = 0.0;
  double TPC_HV_current_measure = 0.0;
  double CPU_temperature = 0.0;
  std::vector<double> env_temperature(MTR_NUM_ENV_SENSOR);
  std::vector<double> env_humidity(MTR_NUM_ENV_SENSOR);
  std::vector<double> env_pressure(MTR_NUM_ENV_SENSOR);
  std::vector<double> acceleration(3);
  std::vector<double> gyro(3);
  std::vector<double> magnet(3);
  double accel_sensor_temperature = 0.0;
  double main_current = 0.0;
  double main_voltage = 0.0;
  uint16_t last_command_index = 0;
  uint16_t last_command_code = 0;
  uint16_t command_reject_count = 0;
  uint64_t software_error_code = 0;
  tree->Branch("start_code", &start_code, "start_code/i");
  tree->Branch("telemetry_type", &telemetry_type, "telemetry_type/s");
  tree->Branch("time_sec", &time_now.tv_sec, "time_sec/I");
  tree->Branch("time_usec", &time_now.tv_usec, "time_usec/I");
  tree->Branch("telemetry_index", &telemetry_index, "telemetry_index/i");
  tree->Branch("run_id", &run_id, "run_id/I");
  tree->Branch("CRC", &crc, "CRC/s");
  tree->Branch("stop_code", &stop_code, "stop_code/i");
  tree->Branch("event_count", &event_count, "event_count/i");
  tree->Branch("current_event_id", &current_event_id, "current_event_id/i");
  tree->Branch("chamber_pressure", &chamber_pressure, "chamber_pressure/D");
  for (int i = 0; i < MTR_NUM_CHAMBER_TEMP; i++) {
    const int j = i + 1;
    tree->Branch(Form("chamber_temperature_%d", j), &chamber_temperature[i], Form("chamber_temperature_%d/D", j));
  }
  tree->Branch("outer_temperature", &outer_temperature, "outer_temperature/D");
  tree->Branch("valve_temperature", &valve_temperature, "valve_temperature/D");
  tree->Branch("TPC_HV_setting", &TPC_HV_setting, "TPC_HV_setting/D");
  tree->Branch("TPC_HV_measure", &TPC_HV_measure, "TPC_HV_measure/D");
  tree->Branch("PMT_HV_setting", &PMT_HV_setting, "PMT_HV_setting/D");
  tree->Branch("TPC_HV_current_measure", &TPC_HV_current_measure, "TPC_HV_current_measure/D");
  tree->Branch("CPU_temperature", &CPU_temperature, "CPU_temperature/D");

  for (int i = 0; i < MTR_NUM_ENV_SENSOR; i++) {
    const int j = i + 1;
    tree->Branch(Form("env_temperature_%d", j), &env_temperature[i], Form("env_temperature_%d/D", j));
    tree->Branch(Form("env_humidity_%d", j), &env_humidity[i], Form("env_humidity_%d/D", j));
    tree->Branch(Form("env_pressure_%d", j), &env_pressure[i], Form("env_pressure_%d/D", j));
  }

  tree->Branch("acceleration_x", &acceleration[0], "acceleration_x/D");
  tree->Branch("acceleration_y", &acceleration[1], "acceleration_y/D");
  tree->Branch("acceleration_z", &acceleration[2], "acceleration_z/D");

  tree->Branch("gyro_x", &gyro[0], "gyro_x/D");
  tree->Branch("gyro_y", &gyro[1], "gyro_y/D");
  tree->Branch("gyro_z", &gyro[2], "gyro_z/D");

  tree->Branch("magnet_x", &magnet[0], "magnet_x/D");
  tree->Branch("magnet_y", &magnet[1], "magnet_y/D");
  tree->Branch("magnet_z", &magnet[2], "magnet_z/D");

  tree->Branch("accel_sensor_temperature", &accel_sensor_temperature, "accel_sensor_temperature/D");

  tree->Branch("main_current", &main_current, "main_current/D");
  tree->Branch("main_voltage", &main_voltage, "main_voltage/D");
  tree->Branch("last_command_code", &last_command_code, "last_command_code/s");
  tree->Branch("last_command_index", &last_command_index, "last_command_index/s");
  tree->Branch("command_reject_count", &command_reject_count, "command_reject_count/s");
  tree->Branch("software_command_error_code", &software_error_code, "software_error_code/l");

  for (int i = 0; i < static_cast<int>(telemetry.size()); i++) {
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
    current_event_id = teldef.CurrentEventID();
    chamber_pressure = pressure_conversion(teldef.ChamberPressure());

    for (int j = 0; j < static_cast<int>(MTR_NUM_CHAMBER_TEMP); j++) {
      chamber_temperature[j] = RTD_conversion(teldef.ChamberTemperature(j));
    }

    valve_temperature = RTD_conversion(teldef.ValveTemperature());
    outer_temperature = RTD_conversion(teldef.OuterTemperature());
    TPC_HV_setting = teldef.TPCHVSetting();
    TPC_HV_measure = slow_ADC_conversion(teldef.TPCHVMeasure());
    PMT_HV_setting = teldef.PMTHVSetting();
    TPC_HV_current_measure = TPC_current_conversion(teldef.TPCHVCurrentMeasure());
    CPU_temperature = teldef.CPUTemperature();

    for (int j = 0; j < MTR_NUM_ENV_SENSOR; j++) {
      env_temperature[j] = teldef.EnvTemperature(j);
      env_humidity[j] = teldef.EnvHumidity(j);
      env_pressure[j] = teldef.EnvPressure(j);
    }

    for (int j = 0; j < 3; j++) {
      acceleration[j] = teldef.Acceleration(j);
      gyro[j] = teldef.Gyro(j);
      magnet[j] = teldef.Magnet(j);
    }

    accel_sensor_temperature = teldef.AccelSensorTemperature();
    main_current = main_current_conversion(teldef.MainCurrent());
    main_voltage = main_voltage_conversion(teldef.MainVoltage());
    last_command_index = teldef.LastCommandIndex();
    last_command_code = teldef.LastCommandCode();
    command_reject_count = teldef.CommandRejectCount();
    software_error_code = teldef.SoftwareErrorCode();

    tree->Fill();
  }
  rootfile->cd();
  tree->Write();
  rootfile->Close();

  return 0;
}
