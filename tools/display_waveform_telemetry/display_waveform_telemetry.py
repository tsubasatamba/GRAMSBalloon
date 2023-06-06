#!/usr/bin/env python3

import numpy as np
import matplotlib.pyplot as plt
import sys
from typing import Any, List

dt = 0.05  # us
header_length = 20
footer_length = 12
event_header_length = 12


plt.rcParams['font.family'] = 'Times New Roman'  # 使用するフォント
plt.rcParams['xtick.direction'] = 'in'  # x軸の目盛線が内向き('in')か外向き('out')か双方向か('inout')
plt.rcParams['ytick.direction'] = 'in'  # y軸の目盛線が内向き('in')か外向き('out')か双方向か('inout')
plt.rcParams['xtick.major.width'] = 1.0  # x軸主目盛り線の線幅
plt.rcParams['ytick.major.width'] = 1.0  # y軸主目盛り線の線幅
plt.rcParams['font.size'] = 15.0  # フォントの大きさ
plt.rcParams['axes.linewidth'] = 1.0  # 軸の線幅edge linewidth。囲みの太さ
plt.rcParams["mathtext.fontset"] = "stixsans"


def convert_to_mV(wf: np.ndarray[Any, Any], channel: int) -> np.ndarray:
    adc_range: np.ndarray = np.array([5.5, 5.5, 5.5, 5.5])
    return wf * adc_range[channel] / 65536 * 1000


def get_num_bin(length: int) -> int:
    return (length - header_length - footer_length - event_header_length) // 8


def main() -> None:
    if len(sys.argv)!=2:
        print("USAGE: ./display_waveform_telemetry.py input_filename")
        quit()
    Chennal_name = ["PMT", "TPC_1", "TPC_2", "TPC_3"]
    filename: str = sys.argv[1]

    with open(filename, "rb") as fr:
        binary = fr.read()
    num_bin = get_num_bin(len(binary))
    eventid: int = int.from_bytes(binary[20:24], byteorder="big", signed=False)
    runid: int = int.from_bytes(binary[16:20], byteorder="big", signed=False)
    eventdata = np.zeros((4, num_bin))
    index = 32
    for i in range(4):
        for j in range(num_bin):
            eventdata[i, j] = int.from_bytes(binary[index:index + 2], byteorder="big", signed=True)
            index += 2
    print(num_bin)

    x = np.linspace(0, num_bin * dt, num_bin)

    fig = plt.figure(1, figsize=(6.4 * 2, 4.8 * 2))

    fig.suptitle(f"RunID={runid}, EventID={eventid}")
    axs = [fig.add_subplot(221 + i, xlabel="time [$\\mu s$]", ylabel="Voltage [mV]", title=Chennal_name[i]) for i in range(4)]
    fig.tight_layout()
    for i in range(4):
        axs[i].plot(x, convert_to_mV(eventdata[i], i), linewidth=1.0, color="black")
    fig.savefig(f"{filename.rstrip('.dat')}.png")
    plt.show()


if __name__ == "__main__":
    main()
