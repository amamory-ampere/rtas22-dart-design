#!/bin/bash

# the old power sampler from
#~/repos/power-sampler/build/rt-dag-sampler  > power.csv 2>&1 &
# the new power sampler. use -v to get the values for all power rails
#../../power-sampler/power-sampler -v -o temp_power.csv &
../../power-sampler/power-sampler -o temp_power.csv &

PID=$!
echo "power sample PID: $PID"
sleep 0.1

# launch the apps
app_pid=()
./lenet &
app_pid+=($!)

# wait the apps to finish and kill the power sampler
wait "${app_pid[@]}"
sleep 0.1
kill $PID
# usually the last line is not complete due to the kill. 
# thus, this command discards the last line
head -n -1 temp_power.csv > power.csv


