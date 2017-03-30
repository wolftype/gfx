#!/bin/bash

#BUILDS AND RUNS ONLY SINGLE FILE 
echo Compiling and Running $1

DIRECTORY=`dirname $1`
TARGET=`basename $1|cut -d'.' -f1 | sed -e "s|/|_|g"`

echo Directory is $DIRECTORY
echo Target is $TARGET

mkdir -p build
cd build

rm bin/$TARGET

RUN_CMAKE=1
MAKE_VERBOSE=1
USE_GLFW=0

for i
  do
    case $i in
     -q | --quiet)
      MAKE_VERBOSE=0
    ;;
    -c | --cmake)
      RUN_CMAKE=0
    ;;
    -g | --glfw)
      USE_GLFW=1
    ;;
    esac
done

if [ $RUN_CMAKE = 1 ]; then
  echo "RUN_CMAKE = TRUE"
  cmake -DBUILD_EXAMPLES=1 ..
fi

make $TARGET
./bin/$TARGET
