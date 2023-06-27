#!/usr/bin/env python3

from display_HK_telemetry import *
import sys
import os
import glob
import argparse


def get_filename_id(filename):
    id_str = filename.split("_")[-1].replace(".dat", "")
    return int(id_str)


def get_filenames(run_id):
    arr = []
    dirname = os.environ['HOME'] + "/data/telemetry/"
    run_id_str = '{:0=6}'.format(run_id)
    arr = glob.glob(dirname + "telemetry_" + run_id_str + "_*_HK_*.dat")
    arr = sorted(arr, key=get_filename_id)
    print(get_filename_id(arr[0]))
    return arr


if __name__ == "__main__":
    # parser = argparse.ArgumentParser()
    # parser.add_argument("-i", "--run_ids", nargs='*', type=int)
    # parser.add_argument("-y", nargs='*')
    # parser.add_argument("-x", nargs=1, default="receive_time", help="X axis")
    if len(sys.argv) < 3:
        raise ValueError("USAGE: ./display_HK_telemetry_all.py telemetry_key run_ID_1 run_ID_2 ...")
    telemetry_key = sys.argv[1]
    run_ids = [int(i) for i in sys.argv[2:]]
    filenames = []
    for run_id in run_ids:
        arr = get_filenames(run_id)
        for s in arr:
            filenames.append(s)
    print(filenames)
    run(telemetry_key, filenames)
