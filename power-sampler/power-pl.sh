#!/bin/bash
# extracts the power from the PL part

count=0;
total=0; 

# keep only the cpu power column
cut -d',' -f3 power.gplot > pl.txt
avg(){
    awk '{ total += $1; count++ } END { print total/count }' $1
}
# remove the 1st line
tail -n +2  pl.txt > pl2.txt
echo "PL idle: $(avg "pl2.txt")"
highest_power=$(sort -t= -nr -k3 pl2.txt | head -1)
echo "PL highest power: $highest_power"
