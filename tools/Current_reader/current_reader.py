#!/usr/bin/env python3

import pathlib
from ..display_HK_telemetry.display_HK_telemetry import create_telemetry_definition, TELEMETRY_LENGTH

VERBOSE = 0


class current_reader:

    def __init__(self, dirname: str, runID: int) -> None:
        self.runID: int = runID
        self.path = pathlib.Path(dirname)
        self.binary = []
        for i in self.path.glob("telemetry_{:0=6}_*_HK_*.dat".format(runID)):
            with open(i, "rb") as fp:
                byte = fp.read()
                self.bytes.append(byte)
        self.telemtry_difinition = create_telemetry_definition()
    def calcurate
        i = 0
        while 1:
            if VERBOSE >= 2:
                print(f"loop: {i}")
            if i >= len(self.binary) - 1:
                break
            if self.binary[i] == 0xeb and self.binary[i + 1] == 0x90:
                x.append(int.from_bytes(self.binary[i + tel["time_sec"][0]:i + tel["time_sec"][1]], 'big', signed=tel["time_sec"][2]))
                if VERBOSE >= 1:
                    print(f"x[{i}]: {x[-1]}")
                y.append(int.from_bytes(self.binary[i + tel[telemetry_key][0]: i + tel[telemetry_key][1]], "big", signed=tel[telemetry_key][2]))
                if VERBOSE >= 1:
                    print(f"y[{i}]: {y[-1]}")
                i += TELEMETRY_LENGTH
            else:
                i += 1
