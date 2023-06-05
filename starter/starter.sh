#!/bin/zsh 
export LD_LIBRARY_PATH=/home/grams/lib:${LD_LIBRARY_PATH}
export RUBYLIB=/home/grams/lib/ruby:${RUBYLIB}

cd /home/grams/software/GRAMSBalloon/operation/isas_long_run_1



while :
do
id=`date "+%Y%m%d%H%M%S"`
./run.rb  &> output_${id}.log
end_status=$?
echo ${end_status}
if [ "${end_status}" -eq "111" ]
then
exit
fi
done

