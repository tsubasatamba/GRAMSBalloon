#!/bin/zsh 
export LD_LIBRARY_PATH=/home/grams/lib:${LD_LIBRARY_PATH}
export RUBYLIB=/home/grams/lib/ruby:${RUBYLIB}

cd /home/grams/software/GRAMSBalloon/operation/isas_long_run_1
id=`date "+%Y%m%d%H%M%S"`
./run.rb  2>&1 | tee output_${id}.log
