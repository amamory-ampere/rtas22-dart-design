#!/bin/bash

if [ ! -f "$1" ]; then
    echo "ERROR: file not found $1."
    exit 1
fi

# keep only the cpu power column
cut -d',' -f2 "$1" > ps.txt
cut -d',' -f3 "$1" > pl.txt

# split the cpu power by time
sed -n '10,250p' ps.txt > idle-300.txt 
sed -n '400,600p' ps.txt > idle-400.txt 
sed -n '800,1000p' ps.txt > idle-600.txt 
sed -n '1200,1500p' ps.txt > idle-1200.txt 

sed -n '1750,1950p' ps.txt > busy-300.txt 
sed -n '2050,2300p' ps.txt > busy-400.txt 
sed -n '2400,2700p' ps.txt > busy-600.txt 
sed -n '2900,3200p' ps.txt > busy-1200.txt 

# split the power for the PL
sed -n '100,3300p' pl.txt > pl-idle.txt 

avg(){
    awk '{ total += $1; count++ } END { print total/count }' "$1"
}

echo "PS idle 300MHz: $(avg "idle-300.txt")"
echo "PS idle 400MHz: $(avg "idle-400.txt")"
echo "PS idle 600MHz: $(avg "idle-600.txt")"
echo "PS idle 1200MHz: $(avg "idle-1200.txt")"

echo "PS busy 300MHz: $(avg "busy-300.txt")"
echo "PS busy 400MHz: $(avg "busy-400.txt")"
echo "PS busy 600MHz: $(avg "busy-600.txt")"
echo "PS busy 1200MHz: $(avg "busy-1200.txt")"

echo "PL idle: $(avg "pl-idle.txt")"

