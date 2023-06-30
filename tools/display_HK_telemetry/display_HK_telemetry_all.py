#!/usr/bin/env python3

from display_HK_telemetry import run, Telmetry_Definition
import os
import glob
import argparse


def get_filename_id(filename: str) -> int:
    id_str = filename.split("_")[-1].replace(".dat", "")
    return int(id_str)


def get_filenames(run_id: int) -> list[str]:
    arr = []
    dirname = os.environ['HOME'] + "/data/telemetry/"
    run_id_str = '{:0=6}'.format(run_id)
    arr = glob.glob(dirname + "telemetry_" + run_id_str + "_*_HK_*.dat")
    if len(arr) == 0:
        raise FileNotFoundError("HK Telemetry data file not found")
    arr = sorted(arr, key=get_filename_id)
    return arr


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("y", nargs=1, help="Telemetry key of y axis. You can also choise a group.", type=str)
    parser.add_argument("run_ids", nargs='*', type=int, help="RunIDs")
    # group.add_argument("-ir", "--run_id_range", nargs=2, type=int, help="runID range")
    parser.add_argument("-x", nargs=1, default="receive_time_sec", help="Telemetry key of x axis.")

    args = parser.parse_args()
    tel = Telmetry_Definition()
    if args.y[0] in tel.keys():
        telemetry_keys = args.y
        show_limit = tel.telemetry_definition[telemetry_keys[0]].show_limit
    elif args.y[0] in tel.groups.keys():
        telemetry_keys = list(tel.groups[args.y[0]])
        show_limit = tel.get_group_show_limit(args.y[0])
    else:
        raise KeyError("Key not found in telemetry_keys or groups key: " + args.y[0])
    filenames = []
    args.run_ids.sort()
    for run_id in args.run_ids:
        arr = get_filenames(run_id)
        for s in arr:
            filenames.append(s)
    print(telemetry_keys)
    print(filenames)
    run(telemetry_keys, filenames, args.x, show_limit=show_limit)
