#!/bin/bash

cpufreq-set -g userspace
../power-sampler/power-sampler -o temp_power.csv &

sleep 2
# the idle power for each OPP
cpufreq-set -f 299999 
sleep 30
cpufreq-set -f 399999
sleep 30
cpufreq-set -f 599999
sleep 30
cpufreq-set -f 1199999
sleep 30

# launch the apps to create the scenario for busy power
cpufreq-set -f 299999 
yes > /dev/null &
yes > /dev/null &
yes > /dev/null &
yes > /dev/null &
sleep 30
cpufreq-set -f 399999
sleep 30
cpufreq-set -f 599999
sleep 30
cpufreq-set -f 1199999
sleep 30

killall yes
sleep 2
killall power-sampler
# usually the last line is not complete due to the kill. 
# thus, this command discards the last line
head -n -1 temp_power.csv > temp_power2.csv
# prepare data for gnuplot
cut -d',' -f37- temp_power2.csv > power.gplot
