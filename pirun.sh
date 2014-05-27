#!/bin/bash
#calls cd build 
#cmake .. -DSRC<argument> && make argument

DIRECTORY=`echo $1 |cut -d'/' -f1`
FILENAME=`echo $1 |cut -d'/' -f2 |cut -d'.' -f1 | sed -e "s|/|_|g"`
TARGET=${FILENAME}
HOST=192.168.0.2

echo BUILDING $TARGET
mkdir pi_build
cd pi_build
rm bin/${TARGET}
cmake .. -DSRC=../$1 -DRPI=1 
make
echo DEPLOYING $TARGET to $HOST
ssh -p 22 $HOST  "rm -f /tmp/$TARGET"
scp -P 22 bin/$TARGET $HOST:/tmp
ssh -t -p 22 $HOST /tmp/$TARGET
