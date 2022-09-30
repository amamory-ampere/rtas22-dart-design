#!/bin/bash

# the old power sampler from
#~/repos/power-sampler/build/rt-dag-sampler  > power.csv 2>&1 &
# the new power sampler. use -v to get the values for all power rails
#../../power-sampler/power-sampler -v -o temp_power.csv &
./power-sampler -o temp_power.csv &

PID=$!
echo "power sample PID: $PID"
sleep 1

# launch the apps
yes > /dev/null &
yes > /dev/null &
yes > /dev/null &
yes > /dev/null &

sleep 3
killall yes

sleep 1

killall power-sampler
# usually the last line is not complete due to the kill. 
# thus, this command discards the last line
head -n -1 temp_power.csv > overload_power.csv


