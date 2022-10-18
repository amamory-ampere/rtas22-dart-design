#!/bin/bash

cpufreq-set -g userspace
taskset -c 1 ../power-sampler/power-sampler -o temp_power.csv &

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
#export TICKS_PER_US=3
echo "BUSY at 300MHz"
cpufreq-set -f 299999 
# uncomment this to run a specific rt-dag scenario
#~/repos/power-aware-optimization/tools/rt-dag/rt-dag/build/rt_dag ~/repos/power-aware-optimization/tools/rt-dag/rt-dag/cpu-overload.yaml
# this is the workload executed in a loop by rt-dag
taskset -c 3 nice -n -20 ~/repos/power-aware-optimization/tools/rt-dag/rt-dag/build/rt_dag -t 10000000000 &
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

killall rt_dag
sleep 2
killall power-sampler
# usually the last line is not complete due to the kill. 
# thus, this command discards the last line
head -n -1 temp_power.csv > temp_power2.csv
# prepare data for gnuplot
cut -d',' -f37- temp_power2.csv > power-rt-dag.gplot
