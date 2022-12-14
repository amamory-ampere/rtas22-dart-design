#!/bin/bash
set -e

cpufreq-set -g userspace
taskset -c 1 ../power-sampler/power-sampler -o temp_power.csv &
#../power-sampler/power-sampler -o temp_power.csv &

sleep 2
# the idle power for each OPP
echo "IDLE at 300MHz"
cpufreq-set -f 299999 
sleep 10
echo "IDLE at 400MHz"
cpufreq-set -f 399999
sleep 10
echo "IDLE at 600MHz"
cpufreq-set -f 599999
sleep 10
echo "IDLE at 1200MHz"
cpufreq-set -f 1199999
sleep 10

# launch the apps to create the scenario for busy power
echo "BUSY at 300MHz"
cpufreq-set -f 299999 
# place each workload to a different core and at the highest priority, dont use chrt 99 which will cause errors in zcu102
#taskset -c 0 nice -n -20 yes > /dev/null &
#taskset -c 1 nice -n -20 yes > /dev/null &
#taskset -c 2 nice -n -20 yes > /dev/null &
taskset -c 3 nice -n -20 yes > /dev/null &
#yes > /dev/null &
#yes > /dev/null &
#yes > /dev/null &
#yes > /dev/null &
sleep 10
echo "BUSY at 400MHz"
cpufreq-set -f 399999
sleep 10
echo "BUSY at 600MHz"
cpufreq-set -f 599999
sleep 10
echo "BUSY at 1200MHz"
cpufreq-set -f 1199999
sleep 10

killall yes
sleep 2
killall power-sampler
# usually the last line is not complete due to the kill. 
# thus, this command discards the last line
head -n -1 temp_power.csv > temp_power2.csv
# prepare data for gnuplot
cut -d',' -f37- temp_power2.csv > power.gplot
