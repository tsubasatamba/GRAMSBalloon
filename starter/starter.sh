#!/bin/zsh 
export LD_LIBRARY_PATH=/home/grams/lib:${LD_LIBRARY_PATH}
export RUBYLIB=/home/grams/lib/ruby:${RUBYLIB}

cd /home/grams/software/GRAMSBalloon/operation/isas_long_run_1


#while :
for i in 0 1 2 3 4
do
id=`date "+%Y%m%d%H%M%S"`
./run.rb  &> output_${id}.log
end_status=$?
if [ "${end_status}"=="111" ]
then
break
fi
done
