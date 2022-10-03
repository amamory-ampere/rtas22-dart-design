#!/bin/bash

# used to test the sampling frequency

# the old power sampler from
#~/repos/power-sampler/build/rt-dag-sampler  > power.csv 2>&1 &
# the new power sampler. use -v to get the values for all power rails
#../../power-sampler/power-sampler -v -o temp_power.csv &
./power-sampler -o temp_power.csv &

sleep 5
killall power-sampler

samples=$(wc -l < temp_power.csv)
echo "Total samples: $samples"
echo "Sampling frequency is: $(( samples / 5)) Hz"
