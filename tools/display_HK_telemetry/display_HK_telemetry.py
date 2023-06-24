#!/usr/bin/env python3

import matplotlib.pyplot as plt
from typing import Dict, Tuple, List, Callable, Optional
import sys
import numpy as np

TELEMETRY_LENGTH = 140
VERVOSE = 0

plt.rcParams['font.family'] = 'Times New Roman'  # 使用するフォント
plt.rcParams['xtick.direction'] = 'in'  # x軸の目盛線が内向き('in')か外向き('out')か双方向か('inout')
plt.rcParams['ytick.direction'] = 'in'  # y軸の目盛線が内向き('in')か外向き('out')か双方向か('inout')
plt.rcParams['xtick.major.width'] = 1.0  # x軸主目盛り線の線幅
plt.rcParams['ytick.major.width'] = 1.0  # y軸主目盛り線の線幅
plt.rcParams['font.size'] = 15.0  # フォントの大きさ
plt.rcParams['axes.linewidth'] = 1.0  # 軸の線幅edge linewidth。囲みの太さ
plt.rcParams["mathtext.fontset"] = "stixsans"


class Telemetry_Definition:
    def __init__(self, start_index: int, length: int, signed: bool, func: Callable = lambda x: x, show_limit: Optional[Tuple[float, float]] = None) -> None:
        self.__start_index = start_index
        self.__length = length
        self.end_index = start_index + length
        self.signed = signed
        self.func = func
        self.show_limit = show_limit

    @property
    def length(self) -> int:
        return self.__length

    @length.setter
    def length(self, v: int) -> None:
        self.__length = v
        self.end_index = self.__length + self.__start_index

    @property
    def start_index(self) -> int:
        return self.__start_index

    @start_index.setter
    def start_index(self, v: int) -> None:
        self.__start_index = v
        self.end_index = self.__length + self.__start_index


def create_telemetry_definition() -> Dict[str, Telemetry_Definition]:
    telemetry_definition: Dict[str, Telemetry_Definition] = {}
    telemetry_definition["start_code"] = Telemetry_Definition(
        0, 4, False, lambda x: x, None)
    telemetry_definition["telemetry_type"] = Telemetry_Definition(
        4, 2, False, lambda x: x, None)
    telemetry_definition["time_sec"] = Telemetry_Definition(
        6, 4, True, lambda x: x, None)
    telemetry_definition["tiem_usec"] = Telemetry_Definition(
        10, 4, True, lambda x: x, None)
    telemetry_definition["telemetry_index"] = Telemetry_Definition(
        14, 4, False, lambda x: x, None)
    telemetry_definition["run_id"] = Telemetry_Definition(
        18, 4, True, lambda x: x, None)
    telemetry_definition["event_count"] = Telemetry_Definition(
        22, 4, False, lambda x: x, None)
    telemetry_definition["current_event_id"] = Telemetry_Definition(
        26, 4, False, lambda x: x, None)
    telemetry_definition["chamber_pressure"] = Telemetry_Definition(
        30, 2, False, lambda x: convert_chamber_pressure(convert_slow_ADC(x)), (0.9, 1.5))
    telemetry_definition["chamber_temperature_1"] = Telemetry_Definition(
        32, 2, False, convert_RTD, (-200, 30))
    telemetry_definition["chamber_temperature_2"] = Telemetry_Definition(
        34, 2, False, convert_RTD, (-200, 30))
    telemetry_definition["chamber_temperature_3"] = Telemetry_Definition(
        36, 2, False, convert_RTD, (-200, 30))
    telemetry_definition["valve_temperature"] = Telemetry_Definition(
        38, 2, False, convert_RTD, (-50, 30))
    telemetry_definition["outer_temperature"] = Telemetry_Definition(
        40, 2, False, convert_RTD, (-50, 30))
    telemetry_definition["tpc_high_voltage_setting"] = Telemetry_Definition(
        42, 4, True, lambda x: x, (0, 5))
    telemetry_definition["tpc_high_voltage_measurement"] = Telemetry_Definition(
        46, 2, False, lambda x: 2 * x, (0, 3))
    telemetry_definition["pmt_high_voltage_setting"] = Telemetry_Definition(
        48, 4, True, lambda x: x, (0, 5))
    telemetry_definition["tpc_high_voltage_current_measurement"] = Telemetry_Definition(
        52, 54, False, lambda x: x)
    telemetry_definition["cpu_temperature"] = Telemetry_Definition(
        54, 2, True, lambda x: x / 10, (0, 80))
    telemetry_definition["hk_temperature_1"] = Telemetry_Definition(
        56, 2, True, lambda x: x / 10, (-10, 50))
    telemetry_definition["hk_temperature_2"] = Telemetry_Definition(
        58, 2, True, lambda x: x / 10, (-10, 50))
    telemetry_definition["hk_temperature_3"] = Telemetry_Definition(
        60, 2, True, lambda x: x / 10, (-10, 50))
    telemetry_definition["hk_temperature_4"] = Telemetry_Definition(
        62, 2, True, lambda x: x / 10, (-10, 50))
    telemetry_definition["hk_temperature_5"] = Telemetry_Definition(
        64, 2, True, lambda x: x / 10, (-10, 50))
    telemetry_definition["hk_humidity_1"] = Telemetry_Definition(
        66, 2, False, lambda x: x, (0, 10))
    telemetry_definition["hk_humidity_2"] = Telemetry_Definition(
        68, 2, False, lambda x: x, (0, 10))
    telemetry_definition["hk_humidity_3"] = Telemetry_Definition(
        70, 2, False, lambda x: x, (0, 10))
    telemetry_definition["hk_humidity_4"] = Telemetry_Definition(
        72, 2, False, lambda x: x, (0, 10))
    # telemetry_definition["hk_humidity_5"] = 74, 76, False, lambda x: x,
    telemetry_definition["hk_humidity_5"] = Telemetry_Definition(
        74, 2, False, lambda x: x, (0, 10))
    # telemetry_definition["hk_pressure_1"] = 76, 78, False, lambda x: x,
    telemetry_definition["hk_pressure_1"] = Telemetry_Definition(
        76, 2, False, lambda x: x / 1013.25, (0.9, 1.5))
    # telemetry_definition["hk_pressure_2"] = 78, 80, False, lambda x: x,
    telemetry_definition["hk_pressure_2"] = Telemetry_Definition(
        78, 2, False, lambda x: x / 1013.25, (0.9, 1.5))
    # telemetry_definition["hk_pressure_3"] = 80, 82, False, lambda x: x,
    telemetry_definition["hk_pressure_3"] = Telemetry_Definition(
        80, 2, False, lambda x: x / 1013.25, (0.9, 1.5))
    # telemetry_definition["hk_pressure_4"] = 82, 84, False, lambda x: x,
    telemetry_definition["hk_pressure_4"] = Telemetry_Definition(
        82, 2, False, lambda x: x / 1013.25, (0.9, 1.5))
    # telemetry_definition["hk_pressure_5"] = 84, 86, False, lambda x: x,
    telemetry_definition["hk_pressure_5"] = Telemetry_Definition(
        84, 2, False, lambda x: x / 1013.25, (0.9, 1.5))
    # telemetry_definition["acceleration_x"] = 86, 88, True, lambda x: x,
    telemetry_definition["acceleration_x"] = Telemetry_Definition(86, 2, True)
    # telemetry_definition["acceleration_y"] = 88, 90, True, lambda x: x,
    telemetry_definition["acceleration_y"] = Telemetry_Definition(88, 2, True)
    # telemetry_definition["acceleration_y"] = 90, 92, True, lambda x: x,
    telemetry_definition["acceleration_y"] = Telemetry_Definition(90, 2, True)
    # telemetry_definition["gyro_x"] = 92, 94, True, lambda x: x,
    telemetry_definition["gyro_x"] = Telemetry_Definition(92, 2, True)
    # telemetry_definition["gyro_y"] = 94, 96, True, lambda x: x,
    telemetry_definition["gyro_y"] = Telemetry_Definition(94, 2, True)
    # telemetry_definition["gyro_z"] = 96, 98, True, lambda x: x,
    telemetry_definition["gyro_z"] = Telemetry_Definition(96, 2, True)
    # telemetry_definition["magnet_x"] = 98, 100, True, lambda x: x,
    telemetry_definition["magnet_x"] = Telemetry_Definition(98, 2, True)
    # telemetry_definition["magnet_y"] = 100, 102, True, lambda x: x,
    telemetry_definition["magnet_y"] = Telemetry_Definition(100, 2, True)
    # telemetry_definition["magnet_z"] = 102, 104, True, lambda x: x,
    telemetry_definition["magnet_z"] = Telemetry_Definition(102, 2, True)
    # telemetry_definition["accel_sensor_temperature"] = 104, 106, True, lambda x: x,
    telemetry_definition["accel_sensor_temperature"] = Telemetry_Definition(
        104, 2, True)
    # telemetry_definition["main_current"] = 106, 108, False, lambda x: x,
    telemetry_definition["main_current"] = Telemetry_Definition(
        106, 2, False, convert_main_current, (0, 1))
    # telemetry_definition["main_voltage"] = 108, 110, False, lambda x: 4096 / 5.026 * x
    telemetry_definition["main_voltage"] = Telemetry_Definition(
        108, 2, False, convert_main_voltage, (0, 32))
    # telemetry_definition["last_command_index"] = 110, 114, False, lambda x: x
    telemetry_definition["last_command_index"] = Telemetry_Definition(
        110, 4, False)
    # telemetry_definition["last_command_code"] = 114, 116, False, lambda x: x,
    telemetry_definition["last_command_code"] = Telemetry_Definition(
        114, 2, False,)
    # telemetry_definition["command_reject_count"] = 116, 118, False, lambda x: x,
    telemetry_definition["command_reject_count"] = Telemetry_Definition(
        116, 2, False,)
    # telemetry_definition["software_error_code"] = 118, 126, False, lambda x: x,
    telemetry_definition["software_error_code"] = Telemetry_Definition(
        118, 8, False)
    # telemetry_definition["crc"] = 126, 128, False, lambda x: x,
    telemetry_definition["crc"] = Telemetry_Definition(126, 2, False)
    # telemetry_definition["end_code"] = 128, 132, False, lambda x: x,
    telemetry_definition["end_code"] = Telemetry_Definition(128, 4, False,)
    # telemetry_definition["receive_time_sec"] = 132, 136, True, lambda x: x,
    telemetry_definition["receive_time_sec"] = Telemetry_Definition(
        132, 4, True,)
    # telemetry_definition["receive_time_usec"] = 136, 140, True, lambda x: x,
    telemetry_definition["receive_time_usec"] = Telemetry_Definition(
        136, 4, True)
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


def run(telemetry_keys: List[str], filenames: list[str], x_key: str = "receive_time_sec") -> None:
    runID = filenames[0].split("_")[1]

    if VERVOSE >= 2:
        print(f"Open files: {filenames}")
    binary = read_binary(filename=filenames)
    if len(binary) % TELEMETRY_LENGTH != 0:
        raise ValueError(
            f"Telemetry length is inconsisttent with the files\nFile bytes: {len(binary)}\nTELEMETRY_LENGTH: {TELEMETRY_LENGTH}")
    tel = create_telemetry_definition()
    if telemetry_keys not in tel.keys():
        raise ValueError("Invalid telemetry code")
    i = 0
    x: List[int] = []
    y: List[List[int]] = [[] for i in range(len(telemetry_keys))]
    while 1:
        if VERVOSE >= 2:
            print(f"loop: {i}")
        if i >= len(binary) - 1:
            break
        if binary[i] == 0xeb and binary[i + 1] == 0x90:
            x.append(int.from_bytes(binary[i + tel[x_key].start_index:i + tel[x_key].end_index], 'big', signed=tel[x_key].signed))
            if VERVOSE >= 1:
                print(f"x[{i}]: {x[-1]}")
            for j in range(len(telemetry_keys)):
                y[j].append(tel[telemetry_keys[j]].func(int.from_bytes(binary[i + tel[telemetry_keys[j]].start_index: i + tel[telemetry_keys[j]].end_index], "big", signed=tel[telemetry_keys[j]].signed)))
                if VERVOSE >= 1:
                    print(f"y[{j}]: {y[j][-1]}")
            i += TELEMETRY_LENGTH
        else:
            i += 1

    x_arr = (np.array(x, dtype=float) - x[0])
    y_arr = np.array(list(map(tel[telemetry_keys[i]].func, y)))
    fig = plt.figure(1, figsize=(6.4, 4.8))
    ax = fig.add_subplot(111, xlabel=x_key)
    for i in range(len(telemetry_keys)):
        ax.plot(x_arr, y_arr)
        if tel[telemetry_keys[i]].show_limit is not None:
            ax.set_ylim(tel[telemetry_keys[i]].show_limit[0], tel[telemetry_keys[i]].show_limit[1])
    fig.savefig(f"telemetry_{runID}_{telemetry_keys}.png")
    plt.show()


if __name__ == "__main__":
    if len(sys.argv) < 3:
        raise ValueError("The arguments must be larger than 2.")
    run(sys.argv[1], sys.argv[2:])


def convert_chamber_pressure(v: float) -> float:
    current = v / 100. * 1000.
    return (current - 4.) / 16. * 2.


def convert_slow_ADC(v: float) -> float:
    return v / 4096. * 5.026


def convert_RTD(v: float) -> float:
    Rref = 430
    return (v / 400 * Rref) / 32.0 - 256


def convert_main_current(v: float) -> float:
    return (convert_slow_ADC(v) - 1) * 1.25


def convert_main_voltage(v: float) -> float:
    return convert_slow_ADC(v) * 24 / 3.34
