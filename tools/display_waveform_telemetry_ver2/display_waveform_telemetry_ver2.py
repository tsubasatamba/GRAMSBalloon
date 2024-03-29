#!/usr/bin/env python3

import numpy as np
import matplotlib.pyplot as plt
import sys
from typing import Any, List
import os

header_length = 22
event_header_length = 28
footer_length = 14
num_bin = 8192
total_length = header_length+footer_length+event_header_length+num_bin

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


def main() -> None:
    if len(sys.argv)!=2:
        print("USAGE: ./display_waveform_telemetry.py input_filename")
        quit()
    Chennal_name = ["PMT", "TPC_1", "TPC_2", "TPC_3"]
    filename: str = sys.argv[1]

    with open(filename, "rb") as fr:
        binary = fr.read()
    binary_size = len(binary)
    num_events = binary_size//(total_length*8)
    print(f"num_events: {num_events}")
    print(binary_size%total_length)
    for i in range(num_events):
        start_index = total_length * 8 * i
        runid: int = int.from_bytes(binary[start_index+18:start_index+22], byteorder="big", signed=False)
        eventid: int = int.from_bytes(binary[start_index+22:start_index+26], byteorder="big", signed=False)
        sample_frequency = 1E-3 * int.from_bytes(binary[start_index+38:start_index+42], byteorder="big", signed=False)
        dt = 1.0/sample_frequency
        #num_bin: int = int.from_bytes(binary[start_index+46:start_index+50], byteorder="big", signed=False)
        eventdata = np.zeros((4, num_bin))
        for j in range(4):
            for k in range(2):
                index = start_index + total_length*(j*2+k) + 50
                for l in range((num_bin+k)//2):
                    eventdata[j, l+k*(num_bin//2)] = int.from_bytes(binary[index: index+2], byteorder="big", signed=True)
                    index += 2
        x = np.linspace(0, num_bin * dt, num_bin)
        fig = plt.figure(1, figsize=(6.4 * 2, 4.8 * 2))
        fig.suptitle(f"RunID={runid}, EventID={eventid}")
        axs = [fig.add_subplot(221 + j, xlabel="time [$\\mu s$]", ylabel="Voltage [mV]", title=Chennal_name[j]) for j in range(4)]
        fig.tight_layout()
        for j in range(4):
            axs[j].plot(x, convert_to_mV(eventdata[j], j), linewidth=1.0, color="black")
            #axs[j].set_xlim(0.0, 30.0)
        image_filename = f"{filename.rstrip('.dat')}_{i}.png"
        fig.savefig(image_filename)
        os.system(f"open {image_filename}")
        plt.close()


if __name__ == "__main__":
    main()
