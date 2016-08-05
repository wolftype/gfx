#!/bin/bash

mkdir -p build
cd build
cmake -DBUILD_EXAMPLES=1 ..  
make VERBOSE=1 -j3
