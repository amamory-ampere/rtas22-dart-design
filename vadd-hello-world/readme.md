
# Seeting up the host computer

This documents describes how to setup the host computer and the ZCU102 board to run the [`vadd hello_world`](https://github.com/Xilinx/Vitis_Accel_Examples/tree/2020.2/hello_world) example as it is, just to make sure that at least we have a complete design flow for, in a next step, to run more complex acceleration examples.

The base assumption is that Vivado, Vitis, and Petalinux for **v2020.2** are already installed and configured to support [zcu102]() board. However, to run the FPGA acceleration examples we need to run to additional setup, as explained next.

So, the goal is to run the following example in zcu102:

```
git clone https://github.com/Xilinx/Vitis_Accel_Examples.git
cd Vitis_Accel_Examples/hello_world
git checkout 2020.2
```

and we want to run the makefile in this directory. However, the documentation is not clear how to setup the environment to run it. More specificly:

```
make build TARGET=hw DEVICE='?' HOST_ARCH=aarch64 EDGE_COMMON_SW=?
```

From the Makefile documentation it's clear that arguments `TARGET=hw` and `HOST_ARCH=aarch64` have to have these values to run on the actual board. But the arguments `DEVICE` and `EDGE_COMMON_SW` are not clear from the Makefile help. This is what is detailed in the next sections.

## Download the base platform 

The `DEVICE` argument refers to the platform name. For ZCU102 board, this platform is called `xilinx_zcu102_base_202020_1` since we are using 2020.2. Go to the following [link](https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/embedded-platforms/archive-vitis-embedded.html), select the correct version (2020.2), and, under the section *Vitis Embedded Base Platforms*, find the link called [`ZCU102 Base 2020.2`](https://www.xilinx.com/member/forms/download/design-license-zcu102-base.html?filename=xilinx_zcu102_base_202020_1.zip).

Run the following commands to install the platform to its expected location:

```
source /opt/Xilinx/Vivado/2020.2/settings64.sh
wget https://www.xilinx.com/member/forms/download/design-license-zcu102-base.html?filename=xilinx_zcu102_base_202020_1.zip
unzip xilinx_zcu102_base_202020_1.zip $XILINX_VITIS/platforms/
```

## Download sdk and sysroot

The `EDGE_COMMON_SW` argument refers to what Xilinx call *Common images*. It is a prebuilt sysroot and SDK tree to enable cross-compilation. Again, go to the following [link](https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/embedded-platforms/archive-vitis-embedded.html), select the correct version (2020.2), and, under the section 
*Common images for Embedded Vitis platforms*, find the link [`ZYNQMP common image`](https://www.xilinx.com/member/forms/download/xef.html?filename=xilinx-zynqmp-common-v2020.2.tar.gz) to download the correct file.

Run the following commands to install the *common image* to its expected location:

```
source /opt/Xilinx/Vivado/2020.2/settings64.sh
wget https://www.xilinx.com/member/forms/download/xef.html?filename=xilinx-zynqmp-common-v2020.2.tar.gz
mkdir -p /opt/Xilinx/images/2020.2/
tar xzf xilinx-zynqmp-common-v2020.2.tar.gz /opt/Xilinx/images/2020.2/
cd /opt/Xilinx/images/2020.2/
./sdk.sh -y -d ./sdk -p
```

After these steps, sysroot is located in `/opt/Xilinx/images/2020.2/sdk/sysroots/aarch64-xilinx-linux/`. 

## Running the synthesis

At this step, we are ready to build the hardware design with the following command:

```
cd Vitis_Accel_Examples/hello_world
make build TARGET=hw DEVICE='xilinx_zcu102_base_202020_1' HOST_ARCH=aarch64 EDGE_COMMON_SW=/opt/Xilinx/images/2020.2/sdk/
```

The entire synthesis process takes about 15 minutes.

## Compiling the application in the host computer

In this step we compile the application:

```
cd Vitis_Accel_Examples/hello_world
make host TARGET=hw DEVICE='xilinx_zcu102_base_202020_1' HOST_ARCH=aarch64 EDGE_COMMON_SW=/opt/Xilinx/images/2020.2/sdk/
```

This will link to header files located in `/opt/Xilinx/images/2020.2/sdk/sysroots/aarch64-xilinx-linux/usr/include/xrt`. It will generate the executable `hello_world`. Run the following command to check the executable:

```
$ file ./hello_world 
./hello_world: ELF 64-bit LSB shared object, ARM aarch64,
```

## Compiling the application in the ZCU102 board


```
mkdir vadd; cd vadd
wget https://raw.githubusercontent.com/Xilinx/Vitis_Accel_Examples/2020.2/hello_world/src/host.cpp
wget https://raw.githubusercontent.com/Xilinx/Vitis_Accel_Examples/2020.2/common/includes/xcl2/xcl2.cpp
wget https://raw.githubusercontent.com/Xilinx/Vitis_Accel_Examples/2020.2/common/includes/xcl2/xcl2.hpp
g++ -o hello_world xcl2.cpp host.cpp -I/usr/include/xrt -Wall -O0 -g -std=c++11 -fmessage-length=0 -lOpenCL -pthread  -lrt -lstdc++
```


## Transfering the design and the host application to the board

```
$ scp ./hello_world root@<board IP>:~
$ scp ./build_dir.hw.xilinx_zcu102_base_202020_1/vadd.xclbin root@<board IP>:~
vadd.xclbin    
```

```
# ./hello_world vadd.xclbin 
XRT build version: 2.8.0
Build hash: f19a872233fbfe2eb933f25fa3d9a780ced774e5
Build date: 2021-03-30 19:53:41
Git branch: 2020.2
PID: 821
UID: 0
[Wed Sep 28 13:56:53 2022 GMT]
HOST: xilinx-zcu102-2020_2
EXE: /home/root/hello_world
[XRT] ERROR: No devices found
[XRT] ERROR: No devices found
[XRT] ERROR: No devices found
<BASEDIR>/Vitis_Accel_Examples/common/includes/xcl2/xcl2.cpp:34 Error calling err = cl::Platform::get(&platforms), error code is: -1001
```

## Building the OS

You need to setup an OS with Xilinx depedencies to run the acceleration examples and *Petalinux* is used to customize the Linux distribution. But be aware that the learning curve can be steep. If you dont know Petalinux and Yocto and is expecting fast results, it's higly recommened to use a pre-built image. The pre-built images are available [here](https://xilinx-wiki.atlassian.net/wiki/spaces/A/pages/1065451521/2020.2+Release). Then download from the link [`2020.2-zcu102-release.tar.xz`](https://www.xilinx.com/member/forms/download/xef.html?filename=2020.2-zcu102-release.tar.xz). Finally, follow the procedure in the same link to extract and prepare the SD card.


If you are used with the [Petalinux flow](https://xilinx.github.io/Embedded-Design-Tutorials/docs/2021.2/build/html/docs/Introduction/ZynqMPSoC-EDT/6-build-linux-sw-for-ps.html), then it is more convenient to build a complete image, using Yocto image [petalinux-image-everything.bb](https://github.com/Xilinx/meta-petalinux/blob/master/recipes-core/images/petalinux-image-everything.bb). This image will include all possible depedencies (XRT, OpenCL, AI libs) to run the examples.
It is also possible to use the [petalinux-image-minimal.bb](https://github.com/Xilinx/meta-petalinux/blob/master/recipes-core/images/petalinux-image-minimal.bb) recipe for an image with shorter compilation time, but then the following depencies must be added [manually](https://github.com/Xilinx/XRT/issues/3650):

```
IMAGE_INSTALL_append = " opencv-dev"

IMAGE_INSTALL_append = " xrt \
	xrt-dev \
	zocl \
	opencl-clhpp-dev \
	opencl-headers-dev \
"
```

You might also want to add OpenCV since a large number of [acceleration examples use OpenCV](https://github.com/Xilinx/Vitis_Libraries/tree/2020.2/vision).
It is recommened to look at the `petalinux-image-everything.bb` and `petalinux-image-full.bb` recipes to see if there is any other important depedency for your usecase.

An alternative resource to build your own Linux Image is to use [meta-retis](https://github.com/fred-framework/meta-retis) Yocto Layer, that creates other Yocto customized images like `retis-dev-image`, which is built on top of `petalinux-image-minimal`. `meta-retis` goal is to include resourses used for real-time linux setup, like, for instance, the PREEMPT-RT setup.

## Running the example on the ZCU102 board

 - [SD Card preparation](https://xilinx-wiki.atlassian.net/wiki/spaces/A/pages/18842385/How+to+format+SD+card+for+SD+boot)

**TODO**

## Examing the Host code

[OpenCL](https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/OpenCL-Programming) is used in the host side of ZCU102.

**TODO**

## Examing the Device code

**TODO** HLS.

## Other documents

 - https://github.com/Xilinx/Vitis-Tutorials/tree/2020.2/Hardware_Accelerators
 - https://github.com/Xilinx/Vitis_Accel_Examples/tree/2020.2/hello_world
 - https://xilinx.github.io/Vitis_Accel_Examples/2020.2/html/hello_world.html
 - https://docs.xilinx.com/r/2020.2-English/ug1393-vitis-application-acceleration/Installing-the-Vitis-Software-Platform
 - https://github.com/Xilinx/Vitis_Embedded_Platform_Source/blob/2020.2/README.md
 - https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Using-Vitis-Embedded-Platforms?tocId=rhJV73slymTI10Qb9AGwAQ
 - [Creating an Acceleration Platform for Vitis Part One: Creating the Hardware Project for the Acceleration Platform in Vivado](https://support.xilinx.com/s/article/1138208?language=en_US)
 - [Creating an Acceleration Platform for Vitis Part Two: Creating the software project for the Acceleration Platform in PetaLinux](https://support.xilinx.com/s/article/1138667?language=en_US)
 - [How to create Ultra96v2 Linux-based Platform in Xilinx Vitis 2019.2](https://highlevel-synthesis.com/2019/11/07/how-to-create-ultra96v2-linux-based-platform-in-xilinx-vitis-2019-2/)
 - [Using Xilinx Vitis for Embedded Hardware Acceleration - Petalinux](https://gpanders.com/blog/vitis-for-embedded-hardware-acceleration/)

## Next Steps

Here is a list of possible next designs:

 - https://github.com/Xilinx/Vitis-Tutorials/tree/2020.2/Hardware_Accelerators/Design_Tutorials/01-convolution-tutorial
 - [1D FFT](https://xilinx.github.io/Vitis_Libraries/dsp/2020.2/user_guide/L1.html#d-floating-point-fp32-fft)
 - [vision examples](https://xilinx.github.io/Vitis_Libraries/vision/2020.2/overview.html)
 - [vision examples benchmark](https://xilinx.github.io/Vitis_Libraries/vision/2022.1/Benchmark.html#datasets)
 - https://github.com/Xilinx/Vitis_Libraries/tree/master/vision/L2
 - https://github.com/Xilinx/Vitis_Libraries/issues/79
 - https://github.com/Xilinx/Vitis_Libraries/issues/80
 - https://github.com/Xilinx/Vitis_Libraries/issues/113
 - https://www.hackster.io/dsp2/fft-acceleration-with-vitis-237412
 - https://www.hackster.io/bradley-whitlock-ad0rx/xilinx-vitis-2020-2-edge-acceleration-fa8bc8
 - https://www.hackster.io/whitney-knitter/vitis-acceleration-flow-on-kv260-vitis-platform-c3537e
 - https://www.hackster.io/mohammad-hosseinabady2/ultra96v2-linux-based-platform-in-xilinx-vitis-2020-1-06f226
 - [ROS integration](https://github.com/Xilinx/KRS/blob/main/sphinx/source/docs/howto.md)

