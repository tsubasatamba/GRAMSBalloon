#!/usr/bin/env python3

import pathlib
from display_HK_telemetry import create_telemetry_definition, TELEMETRY_LENGTH
import sys
VERBOSE = 3
if VERBOSE >= 2:
    import matplotlib.pyplot as plt


def current_time(dirname: str, runID: int, ) -> float:
    telemetry_key = "main_current"
    path = pathlib.Path(dirname)
    binary = bytes()
    globbed = sorted(list(path.glob("telemetry_{:0=6}_*_HK_*.dat".format(runID))))
    for i in globbed:
        with open(i, "rb") as fp:
            binary += fp.read()
    telemetry_difinition = create_telemetry_definition()
    index = 0
    current_time = 0
    t_previous = None

    if VERBOSE >= 2:
        current_time_arr = []
        time_arr = []
        current_arr = []
        fig = plt.figure()
        ax = fig.add_subplot(111, xlabel="time[s]", ylabel="Current_time[As]")
        ax2 = ax.twinx()
        ax2.set_ylabel("Current[A]")
    while 1:
        if VERBOSE >= 2:
            print(f"loop: {index}")
        if index >= len(binary) - 1:
            break
        if binary[index] == 0xeb and binary[index + 1] == 0x90:
            t = int.from_bytes(binary[index + telemetry_difinition["time_sec"][0]:index + telemetry_difinition["time_sec"][1]], 'big', signed=telemetry_difinition["time_sec"][2])
            if t_previous is None:
                t_previous = t
            if VERBOSE >= 1:
                print(f"t[{index}]: {t}")
            current: float = (float(int.from_bytes(binary[index + telemetry_difinition[telemetry_key][0]: index + telemetry_difinition[telemetry_key][1]], "big", signed=telemetry_difinition[telemetry_key][2])) / 4096 * 5.026 - 1) * 1.25
            if VERBOSE >= 1:
                print(f"Current: {current}")
            current_time += current * (t - t_previous)
            t_previous = t
            if VERBOSE >= 2:
                print(f"Current time: {current_time} As")
                current_time_arr.append(current_time)
                current_arr.append(current)
                time_arr.append(t)
                print(f"Current time: {current_time/60/60} Ah")
            index += TELEMETRY_LENGTH
        else:
            index += 1
    if VERBOSE >= 2:
        ax.plot(time_arr, current_time_arr, c="red")
        ax2.plot(time_arr, current_arr, c="blue")
        plt.show()
    return current_time


if __name__ == "__main__":
    dirname = sys.argv[1]
    runID = int(sys.argv[2])
    result = current_time(dirname, runID)
    print(f"dirname: {dirname}")
    print(f"runID: {runID}")
    print(f"result: {result} As")
    print(f"result: {result / 60 / 60} Ah")
