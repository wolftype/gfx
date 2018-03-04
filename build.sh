#!/bin/bash


BUILD_EXAMPLES=0
GFX_USE_GLFW=0
MAKE_VERBOSE=1

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
  esac
done


mkdir -p build
cd build
cmake -DBUILD_EXAMPLES=$BUILD_EXAMPLES -DGFX_USE_GLFW=$GFX_USE_GLFW -DCMAKE_EXPORT_COMPILE_COMMANDS=1 ..
make VERBOSE=$MAKE_VERBOSE -j3
