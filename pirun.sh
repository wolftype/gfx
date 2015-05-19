#!/bin/bash

mkdir -p build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=toolchains/pi.osx.cmake ..  
make VERBOSE=1 -j3



