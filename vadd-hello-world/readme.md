
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

At this step, we are ready to run the design flow with the following command:

```
cd Vitis_Accel_Examples/hello_world
make build TARGET=hw DEVICE='xilinx_zcu102_base_202020_1' HOST_ARCH=aarch64 EDGE_COMMON_SW=/opt/Xilinx/images/2020.2/sdk/sysroots/aarch64-xilinx-linux/
```

The entire process takes about 15 minutes.

## Building the OS

You need to setup an OS with Xilinx depedencies to run the acceleration examples and *Petalinux* is used to customize the Linux distribution. But be aware that the learning curve can be steep. If you dont know Petalinux and Yocto and is expecting fast results, it's higly recommened to use a pre-built image. The pre-built images are available [here](https://xilinx-wiki.atlassian.net/wiki/spaces/A/pages/1065451521/2020.2+Release). Then download from the link [`2020.2-zcu102-release.tar.xz`](https://www.xilinx.com/member/forms/download/xef.html?filename=2020.2-zcu102-release.tar.xz). Finally, follow the procedure in the same link to extract and prepare the SD card.


If you are used with the Petalinux flow, then it is more convenient to build a complete image, using Yocto image [petalinux-image-everything.bb](https://github.com/Xilinx/meta-petalinux/blob/master/recipes-core/images/petalinux-image-everything.bb). This image will include all possible depedencies (XRT, OpenCL, AI libs) to run the examples.
It is also possible to use the [petalinux-image-minimal.bb](https://github.com/Xilinx/meta-petalinux/blob/master/recipes-core/images/petalinux-image-minimal.bb) recipe for an image with shorter compilation time, but then the following depencies must be added manually:

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



## Other documents

 - https://github.com/Xilinx/Vitis-Tutorials/tree/2020.2/Hardware_Accelerators
 - https://github.com/Xilinx/Vitis_Accel_Examples/tree/2020.2/hello_world
 - https://xilinx.github.io/Vitis_Accel_Examples/2020.2/html/hello_world.html
 - https://docs.xilinx.com/r/2020.2-English/ug1393-vitis-application-acceleration/Installing-the-Vitis-Software-Platform
 - https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Using-Vitis-Embedded-Platforms?tocId=rhJV73slymTI10Qb9AGwAQ
 - 


