#!/usr/bin/env python3

from glob import glob
import time
from typing import List
import os


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
        globbed_filename: List[str] = glob("/Users/grams/data/telemetry/telemetry_{:0=6}_*_HK_*.dat".format (runid))
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
    exe_file = "/Users/grams/software/GRAMSBalloon/tools/make_telemetry_root/build/make_telemetry_root"
    os.system(f"{exe_file} {temp_filelist} {output_root}")
    os.system(f"rm {temp_filelist}") 

if __name__=="__main__":
    run([106], "output.root")
