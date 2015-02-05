#!/bin/bash
#calls cd build 
#cmake .. -DSRC<argument> && make argument

DIRECTORY=`dirname $1`
#TARGET=`echo $1 |cut -d'/' -f3 |cut -d'.' -f1 | sed -e "s|/|_|g"`
TARGET=`basename $1|cut -d'.' -f1 | sed -e "s|/|_|g"`

#DIRECTORY=`echo $1 |cut -d'/' -f1`
#FILENAME=`echo $1 |cut -d'/' -f2 |cut -d'.' -f1 | sed -e "s|/|_|g"`
#TARGET=${FILENAME}
HOST=10.0.1.66 #hmmm, could init_session to set this (call init_session.sh 10.0.1.66)

echo BUILDING $TARGET
mkdir pi_build
cd pi_build
rm bin/${TARGET}
cmake .. -DSRC=../$1 -DRPI=1 
make VERBOSE=1
echo DEPLOYING $TARGET to $HOST
ssh -p 22 $HOST  "rm -f /tmp/$TARGET"
scp -P 22 bin/$TARGET $HOST:/tmp
ssh -t -p 22 $HOST /tmp/$TARGET
