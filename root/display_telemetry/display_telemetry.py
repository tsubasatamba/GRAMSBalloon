import matplotlib.pyplot as plt
from typing import Dict, Tuple, Iterable, List
import sys

TELEMETRY_LENGTH = 124
# TELEMETRY_LENGTH = 132
VERVOSE = 1


def create_telemetry_definition() -> Dict[str, Tuple[int, int]]:
    telemetry_definition: Dict[str, Tuple[int, int]] = {}
    telemetry_definition["start_code"] = (0, 2)
    telemetry_definition["telemetry_type"] = (2, 4)
    telemetry_definition["time"] = (4, 12)
    telemetry_definition["telemetry_index"] = (12, 16)
    telemetry_definition["run_id"] = (16, 20)
    telemetry_definition["event_count"] = (20, 24)
    telemetry_definition["chamber_pressure"] = 24, 26
    telemetry_definition["chamber_temperature_1"] = 26, 28
    telemetry_definition["chamber_temperature_2"] = 28, 30
    telemetry_definition["chamber_temperature_3"] = 30, 32
    telemetry_definition["valve_temperature"] = 32, 34
    telemetry_definition["outer_temperature"] = 34, 36
    telemetry_definition["tpc_high_voltage_setting"] = 36, 40
    telemetry_definition["tpc_high_voltage_measurement"] = 40, 42
    telemetry_definition["pmt_high_voltage_setting"] = 42, 46
    telemetry_definition["pmt_high_voltage_measurement"] = 46, 48
    telemetry_definition["cpu_temperature"] = 48, 50
    telemetry_definition["hktemperature"] = 50, 60
    telemetry_definition["hkhumidity"] = 60, 70
    telemetry_definition["hkpressure"] = 70, 80
    telemetry_definition["acceleration_x"] = 80, 82
    telemetry_definition["acceleration_y"] = 82, 84
    telemetry_definition["acceleration_y"] = 84, 86
    telemetry_definition["gyro_x"] = 86, 88
    telemetry_definition["gyro_y"] = 88, 90
    telemetry_definition["gyro_z"] = 90, 92
    telemetry_definition["magnet_x"] = 92, 94
    telemetry_definition["magnet_y"] = 94, 96
    telemetry_definition["magnet_z"] = 96, 98
    telemetry_definition["accel_sensor_temperature"] = 98, 100
    telemetry_definition["main_current"] = 100, 102
    telemetry_definition["main_voltage"] = 102, 104
    telemetry_definition["last_command_index"] = 104, 108
    telemetry_definition["last_command_code"] = 108, 110
    telemetry_definition["command_reject_count"] = 110, 112
    telemetry_definition["software_error_code"] = 112, 120
    telemetry_definition["crc"] = 120, 122
    telemetry_definition["end_code"] = 122, 124
    telemetry_definition["receive_time"] = 124, 132
    return telemetry_definition


def read_binary(filename: Iterable[str] | str) -> bytes:
    if hasattr(filename, "__iter__"):
        binary = bytes()
        for i in range(len(sys.argv) - 1):
            with open(filename[i], "rb") as fr:
                binary += fr.read()
        return binary
    else:
        with open(filename, "rb") as fr:
            binary = fr.read()
        return binary


def main() -> None:
    if len(sys.argv) < 2:
        raise ValueError("The arguments must be larger than 2.")
    binary = read_binary(filename=sys.argv[1:-1])
    if len(binary) % TELEMETRY_LENGTH != 0:
        raise ValueError("Telemetry length is inconsisttent with the files")
    tel = create_telemetry_definition()
    if not sys.argv[1] in tel.keys():
        raise ValueError
    i = 0
    x: List[int] = []
    y: List[int] = []
    while 1:
        if VERVOSE > 2:
            print(f"loop: {i}")
        if binary[i] == 0xeb and binary[i + 1] == 0x90:
            x.append(int(binary[i + tel["time"][0]:i + tel["time"][1]]))
            if VERVOSE > 1:
                print(f"x[{i}]: {x[-1]}")
            y.append(int(binary[i + tel[sys.argv[0]][0]: i + tel[sys.argv[0]][1]]))
            if VERVOSE > 1:
                print(f"y[{i}]: {y[-1]}")
            i += TELEMETRY_LENGTH
        else:
            i += 1

    fig = plt.figure()
    ax = fig.add_subplot(111, xlabel="Time [s]", ylabel=f"{sys.argv[0]}")
    ax.plot(x, y)
    plt.show()


if __name__ == "__main__":
    main()
