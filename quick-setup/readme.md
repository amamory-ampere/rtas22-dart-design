
# `setup-fred.sh`

Script to quickly check on the board if the FRED-image is working

# OpenCV

quisck example to test the OpenCV setup with a Sobel example from: 

 - https://docs.opencv.org/3.4.3/d3/d94/samples_2cpp_2tutorial_code_2ImgTrans_2Sobel_Demo_8cpp-example.html
 - https://raw.githubusercontent.com/opencv/opencv/3.4/samples/cpp/tutorial_code/ImgTrans/Sobel_Demo.cpp
 - https://opencvguide.readthedocs.io/en/latest/opencvcpp/basics.html#sobel-edge-detection
 - https://opencvguide.readthedocs.io/en/latest/opencvpetalinux/basics.html

## Compile 

It can be compiled with g++:
```
g++ sobel_img.cpp -o sobel `pkg-config --libs opencv`
```

or with CMake:
