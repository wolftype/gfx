#!/bin/bash


BUILD_EXAMPLES=0
GFX_USE_GLFW=0
BUILD_GLV=1
MAKE_VERBOSE=1
BUILD_TYPE=RelWithDebInfo

for i
do
  case $i in
  -g| --glfw)
    GFX_USE_GLFW=1
  ;;
  -x | --examples)
    BUILD_EXAMPLES=1
  ;;
  -q | --quiet)
    MAKE_VERBOSE=0
  ;;
  -d | --debug)
    BUILD_TYPE=Debug
  ;;
  -r | --release)
    BUILD_TYPE=Release
  ;;
  esac
done


mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE DBUILD_EXAMPLES=$BUILD_EXAMPLES -DGFX_USE_GLFW=$GFX_USE_GLFW -DCMAKE_EXPORT_COMPILE_COMMANDS=1 ..
make VERBOSE=$MAKE_VERBOSE -j3
