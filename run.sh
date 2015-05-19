#!/bin/bash

#makes build directory, runs cmake .., and make $Target 

#DIRECTORY=`echo $1 |cut -d'/' -f1`
#FILENAME=`echo $1 |cut -d'/' -f2 |cut -d'.' -f1 | sed -e "s|/|_|g"`
#TARGET=${FILENAME}

mkdir -p build
cd build
cmake ..  
make -j3
