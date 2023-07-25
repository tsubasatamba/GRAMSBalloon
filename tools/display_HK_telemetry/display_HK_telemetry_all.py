#!/usr/bin/env python3

from display_HK_telemetry import run
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
        raise FileNotFoundError(f"HK Telemetry data file(runID: {run_id}) not found")
    arr = sorted(arr)
    return arr


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("y", nargs=1, help="Telemetry key of y axis. You can also choise a group.", type=str)
    parser.add_argument("-tw", "--twinx", help="Telemetry of another y axis", type=str)
    parser.add_argument("run_ids", nargs='*', type=int, help="RunIDs")
    # group.add_argument("-ir", "--run_id_range", nargs=2, type=int, help="runID range")
    parser.add_argument("-x", nargs=1, default=["receive_time_sec"], help="Telemetry key of x axis.")
    parser.add_argument("--type", nargs=1, default=["plot"], help="plot type")

    args = parser.parse_args()
    show_limit_twinx = None
    ylabel = None
    ylabel_twinx = None
    filenames = []
    args.run_ids.sort()
    for run_id in args.run_ids:
        arr = get_filenames(run_id)
        for s in arr:
            filenames.append(s)
    print(f"telemetry_keys: {args.y[0]}")
    print(f"twinx: {args.twinx}")
    print(f"filenames length:{len(filenames)}")
    run(args.y[0], filenames, args.x[0], type=args.type[0], twinx_key=args.twinx)
