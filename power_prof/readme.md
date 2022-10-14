

# Power profiling


# Power profiling for CPUs

The following steps, implemented in the `cpu-power-profiling.sh` script, were done to get the power consumption from the zcu102 processors (PS):
 - The island is set to `userspace` governor
 - Launch the power sampler with &
 - Set idle workload
 - For each frequency
    - Set the CPU frequency
    - Wait for 30 seconds
 - Set high workload using four `yes` processes
 - For each frequency
    - Set the CPU frequency
    - Wait for 30 seconds
 - kill all `yes` processes
 - kill the power sampler


|  Freq (MHz)        | Busy Power (W)  | Idle Power(W)  |
|--------------------|-----------------|----------------|
|      			1200 |               0 | 	       0.59 |
| 			     600 |               0 |           0.33 |
| 			     400 |               0 |           0.00 |
| 			     300 |               0 |           0.00 |


## Power down unused blocks

 - [powerdown RPU and GPU](https://xilinx-wiki.atlassian.net/wiki/spaces/A/pages/1417150870/Power+Down+Unused+Blocks)