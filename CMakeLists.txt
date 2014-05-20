#CMAKE FOR GLFW WINDOWING + GFX RENDERING DEMOS
#call with -DSRC=<filename>

cmake_minimum_required(VERSION 2.0)
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)



project(GFX)
get_filename_component(target ${SRC} NAME_WE)

#add_subdirectory(glfw EXCLUDE_FROM_ALL)
#include_directories(${PROJECT_SOURCE_DIR}/glfw/include/)
include_directories(${PROJECT_SOURCE_DIR}/gfx/)
link_directories(${PROJECT_BINARY_DIR})

find_package(OpenGL REQUIRED)

if(!OPENGL_FOUND)
  message("OPENGL NOT FOUND")
endif()

find_package(GLUT REQUIRED)

if(!GLUT_FOUND)
  message("GLUT NOT FOUND")
endif()


add_executable(${target} ${SRC} )
target_link_libraries(${target} ${GLUT_LIBRARY} ${OPENGL_LIBRARY}) #glfw ${GLFW_LIBRARIES})
set( EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin )

if(APPLE)
option(USE_CLANG "use clang" ON)
endif()

if(USE_CLANG)
  message("using clang compiler")
  SET(CMAKE_CXX_COMPILER "clang++")
  SET(CMAKE_C_COMPILER "clang")
  SET(CMAKE_CXX_FLAGS "-std=c++11 -ftemplate-depth-1200 -Wno-switch")
else()
  SET(CMAKE_CXX_FLAGS "-std=c++0x -ftemplate-depth-1200 -Wno-switch")
endif() 

#message("proj dir: " ${PROJECT_BINARY_DIR})
#message( "bin: " ${EXECUTABLE_OUTPUT_PATH} )
#message("src name: " ${SRC} )
#message("target name: " ${target})
#message ("gfx source dir: " ${GFX_SOURCE_DIR} )
#message("glfw libs:\n" ${GLFW_LIBRARIES})



