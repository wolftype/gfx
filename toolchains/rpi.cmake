#TOOLCHAIN FILE FOR CROSS-COMPILING TO RASPBERRY PI
#MAKE A COPY AND MODIFY TO ADAPT TO YOUR SYSTEM

SET(CMAKE_SYSTEM_NAME Linux)
#set(CMAKE_SYSROOT "$ENV{HOME}/pi-root")
SET(CMAKE_C_COMPILER "$ENV{HOME}/code/cuttlefish/cross/arm-none-linux-gnueabi/bin/arm-linux-gnueabihf-g++")
SET(CMAKE_CXX_COMPILER "$ENV{HOME}/code/cuttlefish/cross/arm-none-linux-gnueabi/bin/arm-linux-gnueabihf-g++")

#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D __raspberry__ -D __LINUX_ALSA__ -std=c++0x")
#SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D __raspberry__ -D __LINUX_ALSA__ -std=c++0x")

set(CMAKE_FIND_ROOT_PATH "$ENV{HOME}/code/cuttlefish/cross/pi_root")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

message("root set to: ${CMAKE_FIND_ROOT_PATH}")
message( "LIBRARIES" )
message( ${EGL_LIBRARY} )
message( ${GLES_LIBRARY} )
message( ${EGL_INCLUDE_DIR} )
message( ${GLES_INCLUDE_DIR} )

