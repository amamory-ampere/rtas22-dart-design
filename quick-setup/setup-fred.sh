#!/bin/bash

# download ready to use FRED design and app
mkdir -p /opt/fredsys
cd /opt/fredsys || exit
wget https://github.com/amamory-ampere/rtas22-dart-design/raw/main/sum-sub/fred/fred.tar.gz
tar xzf fred.tar.gz 
cd
mkdir -p repos/sum_sub
cd repos/sum_sub/ || exit
wget https://raw.githubusercontent.com/amamory-ampere/rtas22-dart-design/main/sum-sub/sw/sum/Makefile
mkdir src; cd src/ || exit
wget https://raw.githubusercontent.com/amamory-ampere/rtas22-dart-design/main/sum-sub/sw/sum/src/main.c
cd ..
make

# launch FRED server
cd /opt/fredsys/ || exit
load_hw 
fred-server &

# launch the app
cd ~/repos/sum_sub || exit
./sum_vec
