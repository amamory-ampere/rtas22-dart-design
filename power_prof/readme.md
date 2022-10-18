

# Power profiling


# Power profiling for CPUs

The following steps, implemented in the `cpu-power-profiling-rt-dag.sh` script, were done to get the power consumption from the zcu102 processors (PS):
 - The island is set to `userspace` governor
 - Launch the power sampler with & and pinned to a specific core
 - Set idle workload
 - For each frequency
    - Set the CPU frequency
    - Wait for 10 seconds
 - Set a high workload using `rt-dag -t`, with high priority and pinned to a specific core
 - For each frequency
    - Set the CPU frequency
    - Wait for 10 seconds
 - kill all `rt-dag` processes
 - kill the power sampler

The `cpu-power-profiling.sh` is similar, but the workload in question is a `yes` process.

Running this script generated the follwing plots:

![Alt text](power.png "power plot for CPUs")

And the following table after running the script `split-average.sh`:

|  Freq (MHz)        | Idle Power (W)  | Busy Power(W)  |
|--------------------|-----------------|----------------|
|      			1200 |         1.68586 | 	    1.82729 |
| 			     600 |         1.65163 |        1.72455 |
| 			     400 |         1.63534 |        1.68220 |
| 			     300 |         1.62216 |        1.65982 |

For instance, this means that at 1200 MHz and a single CPU at 100% would increase the PS power consumption in 1.82729 - 1.68586 = .14143 W. 4 CPUs with a full load would result in .14143 * 4 = .56572 W increase.

# Power profiling PL in idle

Using the same script above, PL idle power is **0.812484**. The PL busy power depends on the last configured IP. For instance, preliminary results shows that a 128x128 mat_mul consumes about 1.2W and a 16x16 mat_mul consumes 0.9W. Another very important thing is the instant FPGA power depends on the **last configuration**. Meaning that, even the IP is not used, it continues to draw the same power compared to when it was being used. The **SOLUTION** would be to substantially change the power model to allow power per task instead of power per island, as it is now.

## Power down unused blocks

 - TBD: [powerdown RPU and GPU](https://xilinx-wiki.atlassian.net/wiki/spaces/A/pages/1417150870/Power+Down+Unused+Blocks)