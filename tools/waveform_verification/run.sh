#! /bin/zsh
files=~/data/daq/DAQ_output_000${1}_*_*.root
./build/waveform_verification ${1}.root ${~files}