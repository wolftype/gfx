#!/bin/bash

#pass in path/to/yourfile.cpp
#makes build directory, runs cmake .., and make $Target 

DIRECTORY=`echo $1 |cut -d'/' -f1`
FILENAME=`echo $1 |cut -d'/' -f2 |cut -d'.' -f1 | sed -e "s|/|_|g"`
TARGET=${FILENAME}

mkdir build
cd build
rm bin/${TARGET}
cmake ..  
make VERBOSE=1 ${TARGET}
bin/${TARGET}
