#! /bin/zsh
for file  in ~/data/daq/DAQ_output_000${1}_*_*.root ;do
./build/waveform_verification $file
done