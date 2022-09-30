

# Power profiling


## Power down unused blocks

 - [powerdown RPU and GPU](https://xilinx-wiki.atlassian.net/wiki/spaces/A/pages/1417150870/Power+Down+Unused+Blocks)



The following steps were done to get the power consumption from the zcu102 processors (PS):
 - The island is set to `userspace` governor
 - Shutdown the PL
 - Set idle workload
 - For each frequency
    - Set the CPU frequency
    - Run power-sampler for 1 minute to get the CSV
 - Set high workload using four `yes` processes
 - For each frequency
    - Set the CPU frequency
    - Run power-sampler for 1 minute to get the CSV


|         Freq (MHz)         | Busy Power (uW) | Idle Power(uW) |
|----------------------------|-----------------|----------------|
|      			1200 |               0 | 	   0.59 |
| 			 600 |               0 |           0.33 |
| 			 400 |               0 |           0.00 |
| 			 300 |               0 |           0.00 |

