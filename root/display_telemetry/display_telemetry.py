import matplotlib.pyplot as plt
from typing import Dict, Tuple, Iterable, List
import sys
import numpy as np

# TELEMETRY_LENGTH = 122
TELEMETRY_LENGTH = 132
VERVOSE = 2


def create_telemetry_definition() -> Dict[str, Tuple[int, int, bool]]:
    telemetry_definition: Dict[str, Tuple[int, int, bool]] = {}
    telemetry_definition["start_code"] = 0, 2, False
    telemetry_definition["telemetry_type"] = 2, 4, False
    telemetry_definition["time_sec"] = 4, 10, True
    telemetry_definition["tiem_usec"] = 10, 12, True
    telemetry_definition["telemetry_index"] = 12, 16, False
    telemetry_definition["run_id"] = 16, 20, True
    telemetry_definition["event_count"] = 20, 24, False
    telemetry_definition["chamber_pressure"] = 24, 26, False
    telemetry_definition["chamber_temperature_1"] = 26, 28, False
    telemetry_definition["chamber_temperature_2"] = 28, 30, False
    telemetry_definition["chamber_temperature_3"] = 30, 32, False
    telemetry_definition["valve_temperature"] = 32, 34, False
    telemetry_definition["outer_temperature"] = 34, 36, False
    telemetry_definition["tpc_high_voltage_setting"] = 36, 40, True
    telemetry_definition["tpc_high_voltage_measurement"] = 40, 42, False
    telemetry_definition["pmt_high_voltage_setting"] = 42, 46, True
    telemetry_definition["pmt_high_voltage_measurement"] = 46, 48, False
    telemetry_definition["cpu_temperature"] = 48, 50, True
    telemetry_definition["hk_temperature_1"] = 50, 52, True
    telemetry_definition["hk_temperature_2"] = 52, 54, True
    telemetry_definition["hk_temperature_3"] = 54, 56, True
    telemetry_definition["hk_temperature_4"] = 56, 58, True
    telemetry_definition["hk_temperature_5"] = 58, 60, True
    telemetry_definition["hk_humidity_1"] = 60, 62, False
    telemetry_definition["hk_humidity_2"] = 62, 64, False
    telemetry_definition["hk_humidity_3"] = 64, 66, False
    telemetry_definition["hk_humidity_4"] = 66, 68, False
    telemetry_definition["hk_humidity_5"] = 68, 70, False
    telemetry_definition["hk_pressure_1"] = 70, 72, False
    telemetry_definition["hk_pressure_2"] = 72, 74, False
    telemetry_definition["hk_pressure_3"] = 74, 76, False
    telemetry_definition["hk_pressure_4"] = 76, 78, False
    telemetry_definition["hk_pressure_5"] = 78, 80, False
    telemetry_definition["acceleration_x"] = 80, 82, True
    telemetry_definition["acceleration_y"] = 82, 84, True
    telemetry_definition["acceleration_y"] = 84, 86, True
    telemetry_definition["gyro_x"] = 86, 88, True
    telemetry_definition["gyro_y"] = 88, 90, True
    telemetry_definition["gyro_z"] = 90, 92, True
    telemetry_definition["magnet_x"] = 92, 94, True
    telemetry_definition["magnet_y"] = 94, 96, True
    telemetry_definition["magnet_z"] = 96, 98, True
    telemetry_definition["accel_sensor_temperature"] = 98, 100, True
    telemetry_definition["main_current"] = 100, 102, False
    telemetry_definition["main_voltage"] = 102, 104, False
    telemetry_definition["last_command_index"] = 104, 108, False
    telemetry_definition["last_command_code"] = 108, 110, False
    telemetry_definition["command_reject_count"] = 110, 112, False
    telemetry_definition["software_error_code"] = 112, 120, False
    telemetry_definition["crc"] = 120, 122, False
    telemetry_definition["end_code"] = 122, 124, False
    telemetry_definition["receive_time"] = 124, 132, True
    return telemetry_definition


def read_binary(filename: List[str]) -> bytes:
    binary = bytes()
    for i in range(len(sys.argv) - 2):
        if VERVOSE >= 2:
            print(f"{filename[i]} opening")
        with open(filename[i], "rb") as fr:
            binary += fr.read()
        if VERVOSE >= 2:
            print(f"{filename[i]} closed")
    if VERVOSE >= 3:
        print(binary)

    return binary


def main() -> None:
    print(sys.argv)
    if len(sys.argv) < 3:
        raise ValueError("The arguments must be larger than 2.")
    if VERVOSE >= 2:
        print(f"Open files: {sys.argv[2:]}")
    binary = read_binary(filename=sys.argv[2:])
    if len(binary) % TELEMETRY_LENGTH != 0:
        raise ValueError(f"Telemetry length is inconsisttent with the files\nFile bytes: {len(binary)}\nTELEMETRY_LENGTH: {TELEMETRY_LENGTH}")
    tel = create_telemetry_definition()
    if not sys.argv[1] in tel.keys():
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
            x.append(int.from_bytes(binary[i + tel["time_sec"][0]:i + tel["tim_sece"][1]], 'big', signed=tel["time_sec"][2]))
            if VERVOSE >= 1:
                print(f"x[{i}]: {x[-1]}")
            y.append(int.from_bytes(binary[i + tel[sys.argv[1]][0]: i + tel[sys.argv[1]][1]], "big", signed=tel[sys.argv[1]][2]))
            if VERVOSE >= 1:
                print(f"y[{i}]: {y[-1]}")
            i += TELEMETRY_LENGTH
        else:
            i += 1

    x_arr = (np.array(x) - x[0]) / 1000
    y_arr = np.array(y)
    fig = plt.figure()
    ax = fig.add_subplot(111, xlabel="Time [s]", ylabel=f"{sys.argv[1]}")
    ax.plot(x_arr, y_arr)
    plt.show()


if __name__ == "__main__":
    main()
