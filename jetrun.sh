
#!/bin/bash
#calls cd build 
#cmake .. -DSRC<argument> && make argument

DIRECTORY=`echo $1 |cut -d'/' -f1`
FILENAME=`echo $1 |cut -d'/' -f2 |cut -d'.' -f1 | sed -e "s|/|_|g"`
TARGET=${FILENAME}
HOST=192.168.0.2

echo BUILDING $TARGET
mkdir jet_build
cd jet_build
rm bin/${TARGET}
cmake .. -DSRC=../$1 -DJETSON=1
make VERBOSE=true
#echo DEPLOYING $TARGET to $HOST
#ssh -p 22 $HOST  "rm -f /tmp/$TARGET"
#scp -P 22 bin/$TARGET $HOST:/tmp
#ssh -t -p 22 $HOST /tmp/$TARGET
