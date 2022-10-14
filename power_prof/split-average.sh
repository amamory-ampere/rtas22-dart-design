#!/bin/bash

count=0;
total=0; 

# keep only the cpu power column
cut -d',' -f2 power.gplot > ps.txt
cut -d',' -f3 power.gplot > pl.txt

# split the cpu power by time
sed -n '100,800p' ps.txt > idle-300.txt 
sed -n '1200,1800p' ps.txt > idle-400.txt 
sed -n '2200,3000p' ps.txt > idle-600.txt 
sed -n '3500,4800p' ps.txt > idle-1200.txt 

sed -n '5200,5800p' ps.txt > busy-300.txt 
sed -n '6000,6700p' ps.txt > busy-400.txt 
sed -n '7200,8000p' ps.txt > busy-600.txt 
sed -n '8400,9500p' ps.txt > busy-1200.txt 

# split the power for the PL
sed -n '100,9500p' pl.txt > pl-idle.txt 

avg(){
    awk '{ total += $1; count++ } END { print total/count }' $1
    # for i in $( awk '{ print $1; }' $1 )
    # do 
    #     total=$(echo $total+$i | bc )
    #     ((count++))
    # done
    #echo "$2: $total / $count" | bc
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

