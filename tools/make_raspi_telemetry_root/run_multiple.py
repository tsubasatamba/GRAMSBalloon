#!/usr/bin/env python3

import glob
import time
from typing import List
import os
import sys

def get_filename_id(filename: str):
    id = filename.split("_")[-1].rstrip(".dat")
    print(id)
    return int(id)

def get_filename_time(filename:str):
    stat = os.stat(filename)
    t = stat.st_mtime # UNIX time
    return t



def run(runIDs: List[int], output_root: str):
    filenames = []
    for runid in runIDs:
        runid_str = str(runid).zfill(6)
        name = os.environ["HOME"] + f"/data/telemetry_raspi/telemetry_{runid_str}_*_HK_*.dat"
        globbed_filename = glob.glob(name)
        globbed_filename = sorted(globbed_filename, key=get_filename_id)
        for path in globbed_filename:
            t = get_filename_time(path)
            t_now = time.time()
            if abs(t-t_now) < 5:
                continue
            filenames.append(path)

    temp_filelist = "temp_filelist.txt"
    with open(temp_filelist, "w") as fp :
        for filename in filenames:
            fp.write(filename + "\n")
    exe_file = os.environ["HOME"] + "/software/GRAMSBalloon/tools/make_raspi_telemetry_root/build/make_raspi_telemetry_root"
    os.system(f"{exe_file} {temp_filelist} {output_root}")
    os.system(f"rm {temp_filelist}")

if __name__=="__main__":
    if len(sys.argv)<=2:
        raise ValueError("USAGE: ./run_multiple.py output_file runI_D1 runID_2 runID_3 ...")
    output_file = sys.argv[1]
    run_ids = sys.argv[2:]
    run(run_ids, output_file)
