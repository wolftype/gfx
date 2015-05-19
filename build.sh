#!/bin/bash

mkdir -p build
cd build
cmake ..  
make VERBOSE=1 -j3
