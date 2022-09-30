#!/bin/bash
#~/repos/power-sampler/build/rt-dag-sampler  > power.csv 2>&1 &
~/repos/rtas22-dart-design/power-sampler/power-sampler -v -o power.csv &

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

