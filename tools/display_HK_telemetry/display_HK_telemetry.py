#!/usr/bin/env python3

import matplotlib.pyplot as plt
from typing import Callable, Optional, Any, Iterable, Union
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


class Telemetry_Property:
    def __init__(self, start_index: int, length: int, signed: bool, func: Callable = lambda x: x, show_limit: tuple[Optional[float], Optional[float]] = (None, None)) -> None:
        self.__start_index = start_index
        self.__length = length
        self.end_index = start_index + length
        self.signed = signed
        self.func = func
        self.show_limit = show_limit
        self.__groups: set[str] = set()

    @property
    def groups(self) -> set[str]:
        return self.__groups

    def append_group(self, group: str) -> None:
        self.__groups.add(group)

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


class Telmetry_Definition():
    def __init__(self) -> None:
        self.telemetry_definition: dict[str, Telemetry_Property] = {}
        self.telemetry_definition["start_code"] = Telemetry_Property(
            0, 4, False, lambda x: x, )
        self.telemetry_definition["telemetry_type"] = Telemetry_Property(
            4, 2, False, lambda x: x, )
        self.telemetry_definition["time_sec"] = Telemetry_Property(
            6, 4, True, lambda x: x, )
        self.telemetry_definition["time_usec"] = Telemetry_Property(
            10, 4, True, lambda x: x,)
        self.telemetry_definition["telemetry_index"] = Telemetry_Property(
            14, 4, False, lambda x: x,)
        self.telemetry_definition["run_id"] = Telemetry_Property(
            18, 4, True, lambda x: x, )
        self.telemetry_definition["event_count"] = Telemetry_Property(
            22, 4, False, lambda x: x,)
        self.telemetry_definition["current_event_id"] = Telemetry_Property(
            26, 4, False, lambda x: x, )
        self.telemetry_definition["chamber_pressure"] = Telemetry_Property(
            30, 2, False, lambda x: convert_chamber_pressure(convert_slow_ADC(x)), (0.9, 1.5))
        self.telemetry_definition["chamber_temperature_1"] = Telemetry_Property(
            32, 2, False, convert_RTD, (-200, 30))
        self.telemetry_definition["chamber_temperature_2"] = Telemetry_Property(
            34, 2, False, convert_RTD, (-200, 30))
        self.telemetry_definition["chamber_temperature_3"] = Telemetry_Property(
            36, 2, False, convert_RTD, (-200, 30))
        self.telemetry_definition["valve_temperature"] = Telemetry_Property(
            38, 2, False, convert_RTD, (-50, 30))
        self.telemetry_definition["outer_temperature"] = Telemetry_Property(
            40, 2, False, convert_RTD, (-50, 30))
        self.telemetry_definition["tpc_high_voltage_setting"] = Telemetry_Property(
            42, 4, True, lambda x: x, (0, 5))
        self.telemetry_definition["tpc_high_voltage_measurement"] = Telemetry_Property(
            46, 2, False, lambda x: 2 * x, (0, 3))
        self.telemetry_definition["pmt_high_voltage_setting"] = Telemetry_Property(
            48, 4, True, lambda x: x, (0, 5))
        self.telemetry_definition["tpc_high_voltage_current_measurement"] = Telemetry_Property(
            52, 54, False, lambda x: x)
        self.telemetry_definition["cpu_temperature"] = Telemetry_Property(
            54, 2, True, lambda x: x / 10, (0, 80))
        self.telemetry_definition["hk_temperature_1"] = Telemetry_Property(
            56, 2, True, lambda x: x / 10, (-10, 50))
        self.telemetry_definition["hk_temperature_2"] = Telemetry_Property(
            58, 2, True, lambda x: x / 10, (-10, 50))
        self.telemetry_definition["hk_temperature_3"] = Telemetry_Property(
            60, 2, True, lambda x: x / 10, (-10, 50))
        self.telemetry_definition["hk_temperature_4"] = Telemetry_Property(
            62, 2, True, lambda x: x / 10, (-10, 50))
        self.telemetry_definition["hk_temperature_5"] = Telemetry_Property(
            64, 2, True, lambda x: x / 10, (-10, 50))
        self.telemetry_definition["hk_humidity_1"] = Telemetry_Property(
            66, 2, False, lambda x: x, (0, 10))
        self.telemetry_definition["hk_humidity_2"] = Telemetry_Property(
            68, 2, False, lambda x: x, (0, 10))
        self.telemetry_definition["hk_humidity_3"] = Telemetry_Property(
            70, 2, False, lambda x: x, (0, 10))
        self.telemetry_definition["hk_humidity_4"] = Telemetry_Property(
            72, 2, False, lambda x: x, (0, 10))
        # telemetry_definition["hk_humidity_5"] = 74, 76, False, lambda x: x,
        self.telemetry_definition["hk_humidity_5"] = Telemetry_Property(
            74, 2, False, lambda x: x, (0, 10))
        # telemetry_definition["hk_pressure_1"] = 76, 78, False, lambda x: x,
        self.telemetry_definition["hk_pressure_1"] = Telemetry_Property(
            76, 2, False, lambda x: x / 1013.25 / 10, (0.9, 1.5))
        # telemetry_definition["hk_pressure_2"] = 78, 80, False, lambda x: x,
        self.telemetry_definition["hk_pressure_2"] = Telemetry_Property(
            78, 2, False, lambda x: x / 1013.25 / 10, (0.9, 1.5))
        # telemetry_definition["hk_pressure_3"] = 80, 82, False, lambda x: x,
        self.telemetry_definition["hk_pressure_3"] = Telemetry_Property(
            80, 2, False, lambda x: x / 1013.25 / 10, (0.9, 1.5))
        # telemetry_definition["hk_pressure_4"] = 82, 84, False, lambda x: x,
        self.telemetry_definition["hk_pressure_4"] = Telemetry_Property(
            82, 2, False, lambda x: x / 1013.25 / 10, (0.9, 1.5))
        # telemetry_definition["hk_pressure_5"] = 84, 86, False, lambda x: x,
        self.telemetry_definition["hk_pressure_5"] = Telemetry_Property(
            84, 2, False, lambda x: x / 1013.25 / 10, (0.9, 1.5))
        # telemetry_definition["acceleration_x"] = 86, 88, True, lambda x: x,
        self.telemetry_definition["acceleration_x"] = Telemetry_Property(86, 2, True)
        # telemetry_definition["acceleration_y"] = 88, 90, True, lambda x: x,
        self.telemetry_definition["acceleration_y"] = Telemetry_Property(88, 2, True)
        # telemetry_definition["acceleration_y"] = 90, 92, True, lambda x: x,
        self.telemetry_definition["acceleration_y"] = Telemetry_Property(90, 2, True)
        # telemetry_definition["gyro_x"] = 92, 94, True, lambda x: x,
        self.telemetry_definition["gyro_x"] = Telemetry_Property(92, 2, True)
        # telemetry_definition["gyro_y"] = 94, 96, True, lambda x: x,
        self.telemetry_definition["gyro_y"] = Telemetry_Property(94, 2, True)
        # telemetry_definition["gyro_z"] = 96, 98, True, lambda x: x,
        self.telemetry_definition["gyro_z"] = Telemetry_Property(96, 2, True)
        # telemetry_definition["magnet_x"] = 98, 100, True, lambda x: x,
        self.telemetry_definition["magnet_x"] = Telemetry_Property(98, 2, True)
        # telemetry_definition["magnet_y"] = 100, 102, True, lambda x: x,
        self.telemetry_definition["magnet_y"] = Telemetry_Property(100, 2, True)
        # telemetry_definition["magnet_z"] = 102, 104, True, lambda x: x,
        self.telemetry_definition["magnet_z"] = Telemetry_Property(102, 2, True)
        # telemetry_definition["accel_sensor_temperature"] = 104, 106, True, lambda x: x,
        self.telemetry_definition["accel_sensor_temperature"] = Telemetry_Property(
            104, 2, True)
        # telemetry_definition["main_current"] = 106, 108, False, lambda x: x,
        self.telemetry_definition["main_current"] = Telemetry_Property(
            106, 2, False, convert_main_current, (0, 1))
        # telemetry_definition["main_voltage"] = 108, 110, False, lambda x: 4096 / 5.026 * x
        self.telemetry_definition["main_voltage"] = Telemetry_Property(
            108, 2, False, convert_main_voltage, (0, 32))
        # telemetry_definition["last_command_index"] = 110, 114, False, lambda x: x
        self.telemetry_definition["last_command_index"] = Telemetry_Property(
            110, 4, False)
        # telemetry_definition["last_command_code"] = 114, 116, False, lambda x: x,
        self.telemetry_definition["last_command_code"] = Telemetry_Property(
            114, 2, False,)
        # telemetry_definition["command_reject_count"] = 116, 118, False, lambda x: x,
        self.telemetry_definition["command_reject_count"] = Telemetry_Property(
            116, 2, False,)
        # telemetry_definition["software_error_code"] = 118, 126, False, lambda x: x,
        self.telemetry_definition["software_error_code"] = Telemetry_Property(
            118, 8, False)
        # telemetry_definition["crc"] = 126, 128, False, lambda x: x,
        self.telemetry_definition["crc"] = Telemetry_Property(126, 2, False)
        # telemetry_definition["end_code"] = 128, 132, False, lambda x: x,
        self.telemetry_definition["end_code"] = Telemetry_Property(128, 4, False,)
        # telemetry_definition["receive_time_sec"] = 132, 136, True, lambda x: x,
        self.telemetry_definition["receive_time_sec"] = Telemetry_Property(
            132, 4, True,)
        # telemetry_definition["receive_time_usec"] = 136, 140, True, lambda x: x,
        self.telemetry_definition["receive_time_usec"] = Telemetry_Property(
            136, 4, True)

        self.groups: dict[str, set[str]] = {}
        self.add_group("sec", ["time_sec", "receive_time_sec"])
        self.add_group("usec", ["time_usec", "receive_time_usec"])
        self.add_group("chamber_temperature", ["chamber_temperature_1", "chamber_temperature_2", "chamber_temperature_3"])
        self.add_group("pressure", ["chamber_pressure", "hk_pressure_3", "hk_pressure_4", "hk_pressure_5"])
        self.add_group("hk_pressure", ["hk_pressure_3", "hk_pressure_4", "hk_pressure_5"])
        self.add_group("hk_temperature", ["hk_temperature_3", "hk_temperature_4", "hk_temperature_5"])
        self.add_group("temperature", ["chamber_temperature_1", "chamber_temperature_2", "chamber_temperature_3", "outer_temperature", "valve_temperature", "hk_temperature_3", "hk_temperature_4", "hk_temperature_5"])

    def __getitem__(self, index: str) -> Telemetry_Property:
        return self.telemetry_definition[index]

    def keys(self) -> Any:
        return self.telemetry_definition.keys()

    def add_group(self, groups: Union[Iterable[str], str], keys: Union[Iterable[str], str]) -> None:
        if isinstance(groups, str):
            groups = {groups, }
        if isinstance(keys, str):
            keys = {keys, }
        for key in keys:
            if key not in self.telemetry_definition.keys():
                raise KeyError("Invalid telemetry key: " + key)
            for group in groups:
                self.telemetry_definition[key].append_group(group)
                if group in self.groups.keys():
                    self.groups[group].add(key)
                else:
                    self.groups[group] = {key}

    def get_group_show_limit(self, group: str) -> tuple[Optional[float], Optional[float]]:
        minimum: float | None = None
        maximum: float | None = None
        for key in self.groups[group]:
            if self.telemetry_definition[key].show_limit is not None:
                if minimum is None:
                    minimum = self.telemetry_definition[key].show_limit[0]
                else:
                    minimum = min((minimum, self.telemetry_definition[key].show_limit[0]))
                if maximum is None:
                    maximum = self.telemetry_definition[key].show_limit[1]
                else:
                    maximum = max((maximum, self.telemetry_definition[key].show_limit[1]))
        return minimum, maximum


def read_binary(filename: list[str]) -> bytes:
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


def run(telemetry_keys: list[str], filenames: list[str], x_key: str = "receive_time_sec", show_limit: tuple[Optional[float], Optional[float]] = (None, None)) -> None:
    runID = filenames[0].split("_")[1]

    if VERVOSE >= 2:
        print(f"Open files: {filenames}")
    binary = read_binary(filename=filenames)
    if len(binary) % TELEMETRY_LENGTH != 0:
        raise ValueError(
            f"Telemetry length is inconsisttent with the files\nFile bytes: {len(binary)}\nTELEMETRY_LENGTH: {TELEMETRY_LENGTH}")
    tel = Telmetry_Definition()
    if not set(telemetry_keys) <= set(tel.keys()):
        raise ValueError("Invalid telemetry code")
    i = 0
    x: list[int] = []
    y: list[list[int]] = [[] for i in range(len(telemetry_keys))]
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
    fig = plt.figure(1, figsize=(6.4, 4.8))
    ax = fig.add_subplot(111, xlabel=x_key)
    for i in range(len(telemetry_keys)):
        ax.plot(x_arr, y[i], label=telemetry_keys[i])
    if show_limit is not None:
        ax.set_ylim(*show_limit)
    ax.legend()
    fig.savefig(f"telemetry_{runID}_{telemetry_keys}.png".replace("[", "").replace("]", "").replace("'", ""))
    plt.show()


if __name__ == "__main__":
    if len(sys.argv) < 3:
        raise ValueError("The arguments must be larger than 2.")
    run([sys.argv[1]], sys.argv[2:])


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
