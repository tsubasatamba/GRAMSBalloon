#! /bin/zsh
now_date=`date "+%m/%d %H:%M %Y"`
echo "Set date to ${now_date}"
ssh grams@raspberrypi.local "sudo date -s \"${now_date}\""
