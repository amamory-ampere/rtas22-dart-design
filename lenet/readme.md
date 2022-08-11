# LeNet CNN

Lenet for MNIST handwritten digit recognition using Vivado hls tool. Accuracy of 97.5938% (8-bit Mode).

## DART/FRED Compatibility

This HLS design has been adapted to work with DART and FRED tools designed by [Retis lab](retis.sssup.it/) in Scuola Sant'anna, Pisa. 

* The [design](https://github.com/amamory-ml/mnist_hls) adapted for DART/FRED;
* The [original](https://github.com/FloyedShen/mnist_hls) design.

## Requirement
* Vivado hls 2020.2
* python 3+
* DART/FRED

## Usage
* Download MNIST dataset from [MNIST](http://yann.lecun.com/exdb/mnist/) and copy the dataset to `./hw/MNIST_DATA/`.

* build hls project
```
# When you are in the project root directory
cd hw
vivado_hls build.tcl
```

* Generate `tanh` table (default: ap_fixed<8,3>)
```
# When you are in the project root directory
cd hw/include/filter
python ./tanh_table.py
```

* copy the `sw` directory into the FPGA for compilation.

* Run and see the results

## Results

Application output:

```
root@xilinx-zcu102-2020_2:~/repos/rtas22-dart-design/lenet/sw# ./run.sh 
power sample PID: 509
hello lenet
Read MNIST DATA...
MNIST DATA is loaded
Read MNIST Label...
MNIST Label is loaded
fred_lib: connected to fred server!
buff: buffer mapped at addresses: 0xffffba762000, length:8192 
buff: buffer mapped at addresses: 0xffffba761000, length:80 
Output for iteration 0, batch 0:
0.688 0.031 0.938 0.938 0.406 0.469 0.906 0.969 0.062 0.469 
Expected idx: 0
Expected: 7
Obtained: 7, (0.96875)
Rate: 1

Output for iteration 1, batch 0:
0.906 0.562 0.781 0.844 0.219 0.750 0.844 0.812 0.344 0.594 
Expected idx: 1
Expected: 2
Obtained: 0, (0.90625)
Rate: 0.5

```

FRED timing:

```
root@xilinx-zcu102-2020_2:~/repos/rtas22-dart-design/lenet/sw# cat log.txt 
0000000022982572: 	fred_sys: hw-task: lenet got slot: 0 of its partition: p0, inserted in fri queue
0000000022983011: 	fred_sys: start rcfg of slot: 0 of partition: p0 for hw-task: lenet
0000000022986742: 	fred_sys: devcfg, slot: 0 of partition: p0 rcfg completed for hw-task: lenet in 3630 us
0000000022986815: 	fred_sys: slot: 0 of partition: p0 started for hw-task: lenet
0000000023017101: 	fred_sys: slot: 0 of partition: p0 completed execution of hw-task: lenet in 30274 us
0000000023017444: 	fred_sys: hw-task: lenet got slot: 0 of its partition: p0, inserted in fri queue
0000000023017463: 	fred_sys: skipping rcfg of slot: 0 of partition: p0 for hw-task: lenet
0000000023017531: 	fred_sys: slot: 0 of partition: p0 started for hw-task: lenet
0000000023047728: 	fred_sys: slot: 0 of partition: p0 completed execution of hw-task: lenet in 30188 us
0000000023047970: 	fred_sys: hw-task: lenet got slot: 0 of its partition: p0, inserted in fri queue
0000000023048125: 	fred_sys: skipping rcfg of slot: 0 of partition: p0 for hw-task: lenet
0000000023048146: 	fred_sys: slot: 0 of partition: p0 started for hw-task: lenet
0000000023078492: 	fred_sys: slot: 0 of partition: p0 completed execution of hw-task: lenet in 30337 us
0000000023078702: 	fred_sys: hw-task: lenet got slot: 0 of its partition: p0, inserted in fri queue
0000000023078805: 	fred_sys: skipping rcfg of slot: 0 of partition: p0 for hw-task: lenet
0000000023078824: 	fred_sys: slot: 0 of partition: p0 started for hw-task: lenet
0000000023108977: 	fred_sys: slot: 0 of partition: p0 completed execution of hw-task: lenet in 30145 us
0000000023109210: 	fred_sys: hw-task: lenet got slot: 0 of its partition: p0, inserted in fri queue
0000000023109250: 	fred_sys: skipping rcfg of slot: 0 of partition: p0 for hw-task: lenet
```


## Resource Usage

 - Device: xczu9egffvb1156-2
 - Board: zcu102

|          Site Type         |  Used | Fixed | Available | Util% |
+----------------------------+-------+-------+-----------+-------+
| CLB LUTs*                  | 15928 |     0 |    274080 |  5.81 |
| Block RAM Tile             |  10.5 |     0 |       912 |  1.15 |
| DSPs                       |     6 |     0 |      2520 |  0.24 |


## TODO

 - Note that the accuracy rate is very lower (+- 20%) than the expected 98%. This indicates that the hardware is deliverying 
wrong results. Perhaps some input/ouput conversion. Further debug is necessary;
 - Implement the sw version of the CNN to compare the speedup and validation purposes.


## Authors

* Alexandre Amory (August 2021), ReTiS Lab, Scuola Sant'Anna, Pisa, Italy. Adaptation to DART/FRED.
* Orignal design by FloyedShen;
