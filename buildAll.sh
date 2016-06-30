#!/bin/bash

mkdir -p build
cd build
cmake -DBUILD_EXAMPLES=1 ..  
make -j3

