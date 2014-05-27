#CMAKE FOR GLUT WINDOWING OR EGL WINDOWING + GFX GL OR GLES RENDERING DEMOS
#call with -DSRC=<filename> and -DRPI=1 or 0 (default)

cmake_minimum_required(VERSION 2.0)

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 OLD)
endif(COMMAND cmake_policy)

project(GFX)
get_filename_component(target ${SRC} NAME_WE)
add_executable(${target} ${SRC} )
set( EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin )

message( "system name is: " ${CMAKE_SYSTEM} )
message( "system processor is: " ${CMAKE_SYSTEM_PROCESSOR} )

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  SET( LINUX 1)
  SET( APPLE 0)
  if(${RPI})
    message( "ON LINUX SYSTEM with RASPI FLAG SET TRUE" )
  else()
    message( "ON LINUX SYSTEM" )   
  endif()
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  SET( LINUX 0)
  SET( APPLE 1 ) 
   message( "ON DARWIN SYSTEM" )  
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake_modules/")

if(${RPI})
  find_package(GLES REQUIRED)
  find_package(EGL REQUIRED)
  message( ${EGL_LIBRARY} )
  message( ${GLES_LIBRARY} )
  message( ${EGL_INCLUDE_DIR} )
  message( ${GLES_INCLUDE_DIR} )
  include_directories($ENV{HOME}/pi-root/opt/vc/include)
  include_directories($ENV{HOME}/pi-root/opt/vc/include/interface/vcos/pthreads)
  include_directories($ENV{HOME}/pi-root/opt/vc/include/interface/vmcs_host/linux)
  link_directories($ENV{HOME}/pi-root/opt/vc/lib)
  link_directories($ENV{HOME}/pi-root/usr/lib)
  link_directories($ENV{HOME}/pi-root/usr/local/lib)
  link_directories($ENV{HOME}/pi-root/lib)
  target_link_libraries(${target} bcm_host vchiq_arm vcos ${EGL_LIBRARY} ${GLES_LIBRARY}) 
else()
  find_package(OpenGL REQUIRED)
  find_package(GLUT REQUIRED)
  target_link_libraries(${target} ${GLUT_LIBRARY} ${OPENGL_LIBRARY}) 
endif()

include_directories(${PROJECT_SOURCE_DIR}/gfx/)
link_directories(${PROJECT_BINARY_DIR})

if(${APPLE})
  message("using clang compiler")
  SET(CMAKE_CXX_COMPILER "clang++")
  SET(CMAKE_C_COMPILER "clang")
  SET(CMAKE_CXX_FLAGS "-std=c++11 -ftemplate-depth-1200 -Wno-switch -Wint-to-pointer-cast")
elseif(${LINUX})
  if(${RPI})
    SET(CMAKE_CXX_COMPILER "arm-linux-gnueabihf-g++")
    SET(CMAKE_C_COMPILER "arm-linux-gnueabihf-g++")
    SET(CMAKE_CXX_FLAGS "-D __raspberry__ -U __linux__  -D __LINUX_ALSA__")
  endif()
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -ftemplate-depth-1200 -Wno-switch -Wint-to-pointer-cast")
endif() 



get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY LINK_DIRECTORIES)
foreach(dir ${dirs})
  message(STATUS "dir='${dir}'")
endforeach()







