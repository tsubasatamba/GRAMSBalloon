#!/usr/bin/env python3

import matplotlib.pyplot as plt
from typing import Dict, Tuple, List, Optional
import sys
import numpy as np

# TELEMETRY_LENGTH = 122
TELEMETRY_LENGTH = 132
VERVOSE = 0

plt.rcParams['font.family'] = 'Times New Roman'  # 使用するフォント
plt.rcParams['xtick.direction'] = 'in'  # x軸の目盛線が内向き('in')か外向き('out')か双方向か('inout')
plt.rcParams['ytick.direction'] = 'in'  # y軸の目盛線が内向き('in')か外向き('out')か双方向か('inout')
plt.rcParams['xtick.major.width'] = 1.0  # x軸主目盛り線の線幅
plt.rcParams['ytick.major.width'] = 1.0  # y軸主目盛り線の線幅
plt.rcParams['font.size'] = 15.0  # フォントの大きさ
plt.rcParams['axes.linewidth'] = 1.0  # 軸の線幅edge linewidth。囲みの太さ
plt.rcParams["mathtext.fontset"] = "stixsans"

def create_telemetry_definition() -> Dict[str, Tuple[int, int, bool, Optional[float]]]:
    telemetry_definition: Dict[str, Tuple[int, int, bool, Optional[float]]] = {}
    telemetry_definition["start_code"] = 0, 2, False, 1
    telemetry_definition["telemetry_type"] = 2, 4, False, 1
    telemetry_definition["time_sec"] = 4, 8, True, 1
    telemetry_definition["tiem_usec"] = 8, 12, True, 1
    telemetry_definition["telemetry_index"] = 12, 16, False, 1
    telemetry_definition["run_id"] = 16, 20, True, 1
    telemetry_definition["event_count"] = 20, 24, False, 1
    telemetry_definition["chamber_pressure"] = 24, 26, False, 1
    telemetry_definition["chamber_temperature_1"] = 26, 28, False, 1
    telemetry_definition["chamber_temperature_2"] = 28, 30, False, 1
    telemetry_definition["chamber_temperature_3"] = 30, 32, False, 1
    telemetry_definition["valve_temperature"] = 32, 34, False, 1
    telemetry_definition["outer_temperature"] = 34, 36, False, 1
    telemetry_definition["tpc_high_voltage_setting"] = 36, 40, True, 1
    telemetry_definition["tpc_high_voltage_measurement"] = 40, 42, False, 1
    telemetry_definition["pmt_high_voltage_setting"] = 42, 46, True, 1
    telemetry_definition["pmt_high_voltage_measurement"] = 46, 48, False, 1
    telemetry_definition["cpu_temperature"] = 48, 50, True, 10
    telemetry_definition["hk_temperature_1"] = 50, 52, True, 10
    telemetry_definition["hk_temperature_2"] = 52, 54, True, 10
    telemetry_definition["hk_temperature_3"] = 54, 56, True, 10
    telemetry_definition["hk_temperature_4"] = 56, 58, True, 10
    telemetry_definition["hk_temperature_5"] = 58, 60, True, 10
    telemetry_definition["hk_humidity_1"] = 60, 62, False, 10.
    telemetry_definition["hk_humidity_2"] = 62, 64, False, 10.
    telemetry_definition["hk_humidity_3"] = 64, 66, False, 10.
    telemetry_definition["hk_humidity_4"] = 66, 68, False, 10.
    telemetry_definition["hk_humidity_5"] = 68, 70, False, 10.
    telemetry_definition["hk_pressure_1"] = 70, 72, False, 10
    telemetry_definition["hk_pressure_2"] = 72, 74, False, 10
    telemetry_definition["hk_pressure_3"] = 74, 76, False, 10
    telemetry_definition["hk_pressure_4"] = 76, 78, False, 10
    telemetry_definition["hk_pressure_5"] = 78, 80, False, 10
    telemetry_definition["acceleration_x"] = 80, 82, True, 10
    telemetry_definition["acceleration_y"] = 82, 84, True, 10
    telemetry_definition["acceleration_y"] = 84, 86, True, 10
    telemetry_definition["gyro_x"] = 86, 88, True, 10
    telemetry_definition["gyro_y"] = 88, 90, True, 10
    telemetry_definition["gyro_z"] = 90, 92, True, 10
    telemetry_definition["magnet_x"] = 92, 94, True, 10
    telemetry_definition["magnet_y"] = 94, 96, True, 10
    telemetry_definition["magnet_z"] = 96, 98, True, 10
    telemetry_definition["accel_sensor_temperature"] = 98, 100, True, 10
    telemetry_definition["main_current"] = 100, 102, False, 1
    telemetry_definition["main_voltage"] = 102, 104, False, 4096 / 5.026
    telemetry_definition["last_command_index"] = 104, 108, False, 1
    telemetry_definition["last_command_code"] = 108, 110, False, 1
    telemetry_definition["command_reject_count"] = 110, 112, False, 1
    telemetry_definition["software_error_code"] = 112, 120, False, 1
    telemetry_definition["crc"] = 120, 122, False, 1
    telemetry_definition["end_code"] = 122, 124, False, 1
    telemetry_definition["receive_time_sec"] = 124, 128, True, 1
    telemetry_definition["receive_time_usec"] = 128, 132, True, 1
    return telemetry_definition


def read_binary(filename: List[str]) -> bytes:
    binary = bytes()
    for i in range(len(filename)):
        if VERVOSE >= 2:
            print(f"{filename[i]} opening")
        with open(filename[i], "rb") as fr:
            binary += fr.read()
        if VERVOSE >= 2:
            print(f"{filename[i]} closed")
    if VERVOSE >= 3:
        print(binary)

    return binary


def run(telemetry_key, filenames) -> None:
    runID = filenames[0].split("_")[1]
    
    if VERVOSE >= 2:
        print(f"Open files: {filenames}")
    binary = read_binary(filename=filenames)
    if len(binary) % TELEMETRY_LENGTH != 0:
        raise ValueError(f"Telemetry length is inconsisttent with the files\nFile bytes: {len(binary)}\nTELEMETRY_LENGTH: {TELEMETRY_LENGTH}")
    tel = create_telemetry_definition()
    if telemetry_key not in tel.keys():
        raise ValueError("Invalid telemetry code")
    i = 0
    x: List[int] = []
    y: List[int] = []
    while 1:
        if VERVOSE >= 2:
            print(f"loop: {i}")
        if i >= len(binary) - 1:
            break
        if binary[i] == 0xeb and binary[i + 1] == 0x90:
            x.append(int.from_bytes(binary[i + tel["time_sec"][0]:i + tel["time_sec"][1]], 'big', signed=tel["time_sec"][2]))
            if VERVOSE >= 1:
                print(f"x[{i}]: {x[-1]}")
            y.append(int.from_bytes(binary[i + tel[telemetry_key][0]: i + tel[telemetry_key][1]], "big", signed=tel[telemetry_key][2]))
            if VERVOSE >= 1:
                print(f"y[{i}]: {y[-1]}")
            i += TELEMETRY_LENGTH
        else:
            i += 1

    x_arr = (np.array(x, dtype=float) - x[0])
    y_arr = np.array(y) / tel[telemetry_key][3]
    fig = plt.figure(1, figsize=(6.4, 4.8))
    ax = fig.add_subplot(111, xlabel="Time [s]", ylabel=f"{telemetry_key}")
    ax.plot(x_arr, y_arr)

    fig.savefig(f"telemetry_{runID}_{telemetry_key}.png")
    plt.show()


if __name__ == "__main__":
    if len(sys.argv) < 3:
        raise ValueError("The arguments must be larger than 2.")
    run(sys.argv[1], sys.argv[2:])