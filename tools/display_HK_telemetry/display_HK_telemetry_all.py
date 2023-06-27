#!/usr/bin/env python3

from display_HK_telemetry import Telmetry_Definition, run
import os
import glob
import argparse


def get_filename_id(filename) -> int:
    id_str = filename.split("_")[-1].replace(".dat", "")
    return int(id_str)


def get_filenames(run_id) -> list[str]:
    arr = []
    dirname = os.environ['HOME'] + "/data/telemetry/"
    run_id_str = '{:0=6}'.format(run_id)
    arr = glob.glob(dirname + "telemetry_" + run_id_str + "_*_HK_*.dat")
    if len(arr) == 0:
        raise FileNotFoundError("HK Telemetry data file not found")
    arr = sorted(arr, key=get_filename_id)
    print(get_filename_id(arr[0]))
    return arr


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("y", nargs=1, help="telemetry key of y axis")
    parser.add_argument("run_ids", nargs='*', type=int, help="runIDs")
    # group.add_argument("-ir", "--run_id_range", nargs=2, type=int, help="runID range")
    parser.add_argument("-x", nargs=1, default="receive_time", help="telemetry key of x axis")

    args = parser.parse_args()

    filenames = []
    for run_id in args.run_ids:
        arr = get_filenames(run_id)
        for s in arr:
            filenames.append(s)
    print(filenames)
    run(args.telemetry_keys, filenames, args.x)
