#!/bin/bash

# the old power sampler from
#~/repos/power-sampler/build/rt-dag-sampler  > power.csv 2>&1 &
# the new power sampler. use -v to get the values for all power rails
#../../power-sampler/power-sampler -v -o temp_power.csv &
taskset -c 1 ../../power-sampler/power-sampler -o temp_power.csv &

PID=$!
echo "power sample PID: $PID"
sleep 1

# launch the apps
app_pid=()
# make sure it is running a free core and w higher prio then other tasks
# WHEN RUNNING W FRED, make sure to run fred server 
# taskset -c 2 chrt -f 99 fred-server &
taskset -c 3 chrt -f 91 ./mat_mult > run.log &
app_pid+=($!)

# wait the apps to finish and kill the power sampler
wait "${app_pid[@]}"
sleep 2
kill $PID
# usually the last line is not complete due to the kill. 
# thus, this command discards the last line
head -n -1 temp_power.csv > power.csv
# prepare for gnuplot
cut -d',' -f37- power.csv > power.gplot

# get the execution times
grep "Time taken" run.log > run_clean.log
cut -d' ' -f7 run_clean.log > exec_time.log
wcet=$(sort -t= -nr -k3 exec_time.log | head -1)
bcet=$(sort -t= -nr -k3 exec_time.log | tail -1)
avg=$(awk '{ total += $1; count++ } END { print total/count }' exec_time.log)
echo "WCET=$wcet s, BCET=$bcet s, average=$avg s"
