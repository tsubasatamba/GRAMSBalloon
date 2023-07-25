#!/usr/bin/env python3

import matplotlib.pyplot as plt
import matplotlib.cm
from typing import Callable, Optional, Any, Iterable, Union, Iterator, Literal
import sys
import numpy as np
import datetime

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
    def __init__(self, name: str, start_index: int, length: int, signed: bool, func: Callable = lambda x: x, show_limit: tuple[Optional[float], Optional[float]] = (None, None)) -> None:
        self.__start_index = start_index
        self.__length = length
        self.end_index = start_index + length
        self.signed = signed
        self.func = func
        self.show_limit = show_limit
        self.__groups: set[str] = set()
        self.name = name

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


class Telemetry_Group():
    def __init__(self, name: str, key: str = None, show_name: Optional[str] = None) -> None:
        self.name = name
        if show_name is None:
            self.show_name = name
        else:
            self.show_name = show_name
        if key is None:
            self.telemetries: set[str] = set()
        else:
            self.telemetries = set([key])

    def add(self, key: str) -> None:
        self.telemetries.add(key)

    def __iter__(self) -> Iterator[str]:
        return self.telemetries.__iter__()


class Telmetry_Definition():
    def __init__(self) -> None:
        self.telemetry_definition: dict[str, Telemetry_Property] = {}
        self.telemetry_definition["start_code"] = Telemetry_Property("start_code", 0, 4, False, lambda x: x, )
        self.telemetry_definition["telemetry_type"] = Telemetry_Property("telemetry_type", 4, 2, False, lambda x: x, )
        self.telemetry_definition["time_sec"] = Telemetry_Property("time[sec]", 6, 4, True, lambda x: x, )
        self.telemetry_definition["time_usec"] = Telemetry_Property("time[$\\mu$sec]", 10, 4, True, lambda x: x,)
        self.telemetry_definition["telemetry_index"] = Telemetry_Property("telemetry_index", 14, 4, False, lambda x: x,)
        self.telemetry_definition["run_id"] = Telemetry_Property("run_id", 18, 4, True, lambda x: x, )
        self.telemetry_definition["event_count"] = Telemetry_Property("event_count", 22, 4, False, lambda x: x,)
        self.telemetry_definition["current_event_id"] = Telemetry_Property("current_event_id", 26, 4, False, lambda x: x, )
        self.telemetry_definition["chamber_pressure"] = Telemetry_Property("chamber_pressure[atm]", 30, 2, False, lambda x: convert_chamber_pressure(convert_slow_ADC(x)), (0.9, 1.5))
        self.telemetry_definition["chamber_temperature_1"] = Telemetry_Property("chamber_temperature_upper[$^\\circ \\rm{C}$]", 32, 2, False, convert_RTD, (-200, 30))
        self.telemetry_definition["chamber_temperature_2"] = Telemetry_Property("chamber_temperature_middle[$^\\circ \\rm{C}$]]", 34, 2, False, convert_RTD, (-200, 30))
        self.telemetry_definition["chamber_temperature_3"] = Telemetry_Property("chamber_temperature_lower[$^\\circ \\rm{C}$]", 36, 2, False, convert_RTD, (-200, 30))
        self.telemetry_definition["valve_temperature"] = Telemetry_Property("valve_temperature[$^\\circ \\rm{C}$]", 38, 2, False, convert_RTD, (-50, 30))
        self.telemetry_definition["outer_temperature"] = Telemetry_Property("outer_temperature[$^\\circ \\rm{C}$]", 40, 2, False, convert_RTD, (-50, 30))
        self.telemetry_definition["tpc_high_voltage_setting"] = Telemetry_Property("tpc_high_voltage_setting[V]", 42, 4, True, lambda x: x, (0, 5))
        self.telemetry_definition["tpc_high_voltage_measurement"] = Telemetry_Property("tpc_high_voltage_measurement[kV]", 46, 2, False, lambda x: 2 * x, (0, 3))
        self.telemetry_definition["pmt_high_voltage_setting"] = Telemetry_Property("pmt_high_voltage_setting[V]", 48, 4, True, lambda x: x, (0, 5))
        self.telemetry_definition["tpc_high_voltage_current_measurement"] = Telemetry_Property("tpc_high_voltage_current_measurement[$\\rm{\\mu}$A]", 52, 54, False, lambda x: x)
        self.telemetry_definition["cpu_temperature"] = Telemetry_Property("cpu_temperature[$^\\circ \\rm{C}$]", 54, 2, True, lambda x: x / 10, (0, 80))
        self.telemetry_definition["hk_temperature_1"] = Telemetry_Property("hk_temperature_1[$^\\circ \\rm{C}$]", 56, 2, True, lambda x: x / 10, (-10, 50))
        self.telemetry_definition["hk_temperature_2"] = Telemetry_Property("hk_temperature_2[$^\\circ \\rm{C}$]", 58, 2, True, lambda x: x / 10, (-10, 50))
        self.telemetry_definition["hk_temperature_3"] = Telemetry_Property("hk_temperature_3[$^\\circ \\rm{C}$]", 60, 2, True, lambda x: x / 10, (-10, 50))
        self.telemetry_definition["hk_temperature_4"] = Telemetry_Property("hk_temperature_4[$^\\circ \\rm{C}$]", 62, 2, True, lambda x: x / 10, (-10, 50))
        self.telemetry_definition["hk_temperature_5"] = Telemetry_Property("hk_temperature_5[$^\\circ \\rm{C}$]", 64, 2, True, lambda x: x / 10, (-10, 50))
        self.telemetry_definition["hk_humidity_1"] = Telemetry_Property("hk_humidity_1[%]", 66, 2, False, lambda x: x, (0, 10))
        self.telemetry_definition["hk_humidity_2"] = Telemetry_Property("hk_humidity_2[%]", 68, 2, False, lambda x: x, (0, 10))
        self.telemetry_definition["hk_humidity_3"] = Telemetry_Property("hk_humidity_3[%]", 70, 2, False, lambda x: x, (0, 10))
        self.telemetry_definition["hk_humidity_4"] = Telemetry_Property("hk_humidity_4[%]", 72, 2, False, lambda x: x, (0, 10))
        self.telemetry_definition["hk_humidity_5"] = Telemetry_Property("hk_humidity_5[%]", 74, 2, False, lambda x: x, (0, 10))
        self.telemetry_definition["hk_pressure_1"] = Telemetry_Property("hk_pressure_1[atm]", 76, 2, False, lambda x: x / 1013.25 / 10, (0.9, 1.5))
        self.telemetry_definition["hk_pressure_2"] = Telemetry_Property("hk_pressure_2[atm]", 80, 2, False, lambda x: x / 1013.25 / 10, (0.9, 1.5))
        self.telemetry_definition["hk_pressure_3"] = Telemetry_Property("hk_pressure_3[atm]", 78, 2, False, lambda x: x / 1013.25 / 10, (0.9, 1.5))
        self.telemetry_definition["hk_pressure_4"] = Telemetry_Property("hk_pressure_4[atm]", 82, 2, False, lambda x: x / 1013.25 / 10, (0.9, 1.5))
        self.telemetry_definition["hk_pressure_5"] = Telemetry_Property("hk_pressure_5[atm]", 84, 2, False, lambda x: x / 1013.25 / 10, (0.9, 1.5))
        self.telemetry_definition["acceleration_x"] = Telemetry_Property("acceleration_x[G]", 86, 2, True)
        self.telemetry_definition["acceleration_y"] = Telemetry_Property("acceleration_y[G]", 88, 2, True)
        self.telemetry_definition["acceleration_z"] = Telemetry_Property("acceleration_z[G]", 90, 2, True)
        self.telemetry_definition["gyro_x"] = Telemetry_Property("gyro_x[dps]", 92, 2, True)
        self.telemetry_definition["gyro_y"] = Telemetry_Property("gyro_y[dps]", 94, 2, True)
        self.telemetry_definition["gyro_z"] = Telemetry_Property("gyro_z[dps]", 96, 2, True)
        self.telemetry_definition["magnet_x"] = Telemetry_Property("magnet_x[$\\mu$T]", 98, 2, True)
        self.telemetry_definition["magnet_y"] = Telemetry_Property("magnet_y[$\\mu$T]", 100, 2, True)
        self.telemetry_definition["magnet_z"] = Telemetry_Property("magnet_z[$\\mu$T]", 102, 2, True)
        self.telemetry_definition["accel_sensor_temperature"] = Telemetry_Property("accel_sensor_temperature[$^\\circ \\rm{C}$]", 104, 2, True)
        self.telemetry_definition["main_current"] = Telemetry_Property("main_current[A]", 106, 2, False, convert_main_current, (0, 1))
        self.telemetry_definition["main_voltage"] = Telemetry_Property("main_voltage[A]", 108, 2, False, convert_main_voltage, (0, 32))
        self.telemetry_definition["last_command_index"] = Telemetry_Property("last_command_index", 110, 4, False)
        self.telemetry_definition["last_command_code"] = Telemetry_Property("last_command_code", 114, 2, False,)
        self.telemetry_definition["command_reject_count"] = Telemetry_Property("command_reject_count", 116, 2, False,)
        self.telemetry_definition["software_error_code"] = Telemetry_Property("software_error_code", 118, 8, False)
        self.telemetry_definition["crc"] = Telemetry_Property("crc", 126, 2, False)
        self.telemetry_definition["end_code"] = Telemetry_Property("end_code", 128, 4, False,)
        self.telemetry_definition["receive_time_sec"] = Telemetry_Property("receive_time[sec]", 132, 4, True,)
        self.telemetry_definition["receive_time_usec"] = Telemetry_Property("receive_time[$\\mu$sec]", 136, 4, True)

        self.groups: dict[str, Telemetry_Group] = {}
        self.add_group("sec", ["time_sec", "receive_time_sec"])
        self.add_group("usec", ["time_usec", "receive_time_usec"])
        self.add_group("chamber_temperature", ["chamber_temperature_1", "chamber_temperature_2", "chamber_temperature_3"])
        self.add_group("pressure", ["chamber_pressure", "hk_pressure_3", "hk_pressure_4", "hk_pressure_5"])
        self.add_group("hk_pressure", ["hk_pressure_3", "hk_pressure_4", "hk_pressure_5"])
        self.add_group("hk_temperature", ["hk_temperature_3", "hk_temperature_4", "hk_temperature_5"])
        self.add_group("temperature", ["chamber_temperature_1", "chamber_temperature_2", "chamber_temperature_3", "outer_temperature", "valve_temperature", "hk_temperature_3", "hk_temperature_4", "hk_temperature_5"])
        self.groups["pressure"].show_name = "pressure[atm]"
        self.groups["temperature"].show_name = "temperature[$^\\circ \\rm{C}$]"
        self.groups["hk_pressure"].show_name = "hk_pressure[atm]"
        self.groups["hk_temperature"].show_name = "hk_temperature[$^\\circ \\rm{C}$]"
        self.groups["chamber_temperature"].show_name = "chamber_temperature[$^\\circ \\rm{C}$]"

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
                    self.groups[group] = Telemetry_Group(group, key=key)

    def get_group_show_limit(self, group: str) -> tuple[Optional[float], Optional[float]]:
        minimum: float | None = None
        maximum: float | None = None
        for key in self.groups[group].telemetries:
            if self.telemetry_definition[key].show_limit is not None:
                if minimum is None:
                    minimum = self.telemetry_definition[key].show_limit[0]
                else:
                    minimum = min((minimum, self.telemetry_definition[key].show_limit[0]))  # type: ignore
                if maximum is None:
                    maximum = self.telemetry_definition[key].show_limit[1]
                else:
                    maximum = max((maximum, self.telemetry_definition[key].show_limit[1]))  # type: ignore
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


def run(telemetry_key: str, filenames: list[str], x_key: str = "receive_time_sec", type: Literal["plot", "scatter"] = "plot", twinx_key: Optional[str] = None,) -> None:
    runID = filenames[0].split("_")[-4]
    _colormap = matplotlib.cm.get_cmap("tab20")
    tel = Telmetry_Definition()
    if telemetry_key in tel.keys():
        show_limit = tel.telemetry_definition[telemetry_key].show_limit
        telemetry_keys = [telemetry_key]
        ylabel = tel[telemetry_key].name
    elif telemetry_key in tel.groups.keys():
        telemetry_keys = list(tel.groups[telemetry_key])
        show_limit = tel.get_group_show_limit(telemetry_key)
        ylabel = tel.groups[telemetry_key].show_name
    else:
        raise KeyError("Key not found in telemetry_keys or groups key: " + telemetry_key)
    twinx = None
    show_limit_twinx = None
    if twinx_key is not None:
        if twinx_key in tel.keys():
            twinx = [twinx_key]
            show_limit_twinx = tel.telemetry_definition[twinx_key].show_limit
            ylabel_twinx = tel[twinx_key].name
        elif twinx_key in tel.groups.keys():
            twinx = list(tel.groups[twinx_key])
            show_limit_twinx = tel.get_group_show_limit(twinx_key)
            ylabel_twinx = tel.groups[twinx_key].show_name
        else:
            raise KeyError("Key of twinx not found in telemetry_keys or groups key: " + twinx_key)

    if VERVOSE >= 2:
        print(f"Open files: {filenames}")
    binary = read_binary(filename=filenames)
    if len(binary) % TELEMETRY_LENGTH != 0:
        raise ValueError(
            f"Telemetry length is inconsisttent with the files\nFile bytes: {len(binary)}\nTELEMETRY_LENGTH: {TELEMETRY_LENGTH}")
    i = 0
    x: list[int] = []
    y: list[list[int]] = [[] for i in range(len(telemetry_keys))]
    if twinx is not None:
        y2: list[list[int]] = [[] for i in range(len(twinx))]
    while 1:
        if VERVOSE >= 2:
            print(f"loop: {i}")
        if i >= len(binary) - 1:
            break
        if binary[i] == 0xeb and binary[i + 1] == 0x90:
            x.append(tel[x_key].func(int.from_bytes(binary[i + tel[x_key].start_index:i + tel[x_key].end_index], 'big', signed=tel[x_key].signed)))
            if VERVOSE >= 1:
                print(f"x[{i}]: {x[-1]}")
            for j in range(len(telemetry_keys)):
                y[j].append(tel[telemetry_keys[j]].func(int.from_bytes(binary[i + tel[telemetry_keys[j]].start_index: i + tel[telemetry_keys[j]].end_index], "big", signed=tel[telemetry_keys[j]].signed)))
                if VERVOSE >= 1:
                    print(f"y[{j}][{i}]: {y[j][-1]}")
            if twinx is not None:
                for j in range(len(twinx)):
                    y2[j].append(tel[twinx[j]].func(int.from_bytes(binary[i + tel[twinx[j]].start_index:i + tel[twinx[j]].end_index], "big", signed=tel[twinx[j]].signed)))
                    if VERVOSE >= 1:
                        print(f"y2[{j}][{i}]: {y2[j][-1]}")
            i += TELEMETRY_LENGTH
        else:
            i += 1
    if x_key.find("time_sec") >= 0:
        x_arr = np.array(list(map(datetime.datetime.fromtimestamp, x)), dtype=np.datetime64)
    else:
        x_arr = np.array(x, dtype=float)
    fig = plt.figure(1, figsize=(6.4, 4.8))
    ax = fig.add_subplot(111, xlabel=tel[x_key].name)
    _colormap_index = 0

    if ylabel is not None:
        ax.set_ylabel(ylabel)
    if twinx is not None:
        ax2 = ax.twinx()
        if twinx_key is not None:
            ax2.set_ylabel(ylabel_twinx)
    for i in range(len(telemetry_keys)):
        if type == "plot":
            ax.plot(x_arr, y[i], label=telemetry_keys[i], c=_colormap.colors[_colormap_index])
            _colormap_index += 1
        elif type == "scatter":
            ax.scatter(x_arr, y[i], label=telemetry_keys[i], s=0.1, c=_colormap.colors[_colormap_index])
            _colormap_index += 1
    if twinx is not None:
        for i in range(len(twinx)):
            if type == "plot":
                ax2.plot(x_arr, y2[i], label=twinx[i], linestyle="dashdot", c=_colormap.colors[_colormap_index])
                _colormap_index += 1
            elif type == "scatter":
                if twinx is not None:
                    ax2.scatter(x_arr, y2[i], label=twinx[i], s=0.1, c=_colormap.colors[_colormap_index])
                    _colormap_index += 1
    if show_limit is not None:
        ax.set_ylim(*show_limit)
    if (show_limit_twinx is not None) and (twinx is not None):
        ax2.set_ylim(*show_limit_twinx)
    if twinx is not None:
        lines1, labels1 = ax.get_legend_handles_labels()
        lines2, labels2 = ax2.get_legend_handles_labels()
        ax2.legend(lines1 + lines2, labels1 + labels2)
    else:
        ax.legend()
    fig.savefig(f"telemetry_{runID}_{telemetry_keys}.png".replace("[", "").replace("]", "").replace("'", ""))
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
