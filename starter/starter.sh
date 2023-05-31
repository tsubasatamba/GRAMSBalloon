#!/bin/zsh 
export LD_LIBRARY_PATH=/home/grams/lib:${LD_LIBRARY_PATH}
export RUBYLIB=/home/grams/lib/ruby:${RUBYLIB}

cd /home/grams/software/GRAMSBalloon/examples/mt_test4
id=`date "+%Y%m%d%H%M%S"`
./mt_test4.rb  2>&1 | tee output_${id}.log
