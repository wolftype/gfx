#TOOLCHAIN FILE FOR CROSS-COMPILING TO RASPBERRY PI
#MAKE A COPY AND MODIFY TO ADAPT TO YOUR SYSTEM's Cross Tools
set(RASPBERRY 1)
set(DEFINITIONS "-D __raspberry__")

SET(CMAKE_SYSTEM_NAME Linux)
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D __raspberry__ -D __LINUX_ALSA__ -std=c++0x")
#SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D __raspberry__ -D __LINUX_ALSA__ -std=c++0x")

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_C_COMPILER   ${CMAKE_SOURCE_DIR}/cross/arm-none-linux-gnueabi/bin/arm-none-linux-gnueabi-gcc)
set(CMAKE_CXX_COMPILER ${CMAKE_SOURCE_DIR}/cross/arm-none-linux-gnueabi/bin/arm-none-linux-gnueabi-g++)
set(CMAKE_FIND_ROOT_PATH ${CMAKE_SOURCE_DIR}/cross/pi-root)
#set(CMAKE_SYSROOT ${CMAKE_SOURCE_DIR}/cross/pi-root)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)


message("pi_root set to: ${PI_ROOT}")

message( "LIBRARIES" )
message( ${EGL_LIBRARY} )
message( ${GLES_LIBRARY} )
message( ${EGL_INCLUDE_DIR} )
message( ${GLES_INCLUDE_DIR} )

