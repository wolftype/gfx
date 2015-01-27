#CMAKE FOR GLUT WINDOWING OR EGL WINDOWING + GFX GL OR GLES RENDERING DEMOS
#call with -DSRC=<filename> and -DRPI=1 or 0 (default)

#VERSION
cmake_minimum_required(VERSION 2.0)

#POLICIES
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 OLD)
endif(COMMAND cmake_policy)


#SYSTEM
message( "system name is: " ${CMAKE_SYSTEM} )
message( "system processor is: " ${CMAKE_SYSTEM_PROCESSOR} )
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  SET( LINUX 1)
  SET( APPLE 0)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  SET( LINUX 0)
  SET( APPLE 1 ) 
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  SET( WINDOWS 1)
endif()

#PROJECT
project(GFX)
#TARGET
get_filename_component(target ${SRC} NAME_WE)

add_executable(${target} ${SRC} )
set( EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin )


#PACKAGES
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake_modules/")

#WINDOWING
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
elseif(${JETSON})

  option(USE_CLANG "use clang" ON)
  find_package(OpenGL REQUIRED)
  find_package(GLEW REQUIRED)
  find_package(GLUT REQUIRED)
  message( ${OPENGL_LIBRARY} )
  message( ${GLEW_LIBRARY} )
  message( ${OPENGL_INCLUDE_DIR} )
  message( ${GLEW_INCLUDE_DIR} )
  message( ${GLUT_LIBRARY} )
  message( ${GLUT_INCLUDE_DIR} )
  include_directories(${GLEW_INCLUDE_DIR} )
  target_link_libraries(${target} ${GLUT_LIBRARY} ${OPENGL_LIBRARY} ${GLEW_LIBRARY})
else()
  find_package(OpenGL REQUIRED)
  find_package(Glew REQUIRED)
  find_package(GLUT REQUIRED)
  target_link_libraries(${target} ${GLUT_LIBRARY} ${OPENGL_LIBRARY} ${GLEW_LIBRARY} ) 
  
  #GLFW  
#  set( GLFW_BUILD_EXAMPLES OFF CACHE BOOL "turn off build examples") 
#  add_subdirectory(glfw)
#  link_directories(${PROJECT_BINARY_DIR})
#  link_directories(${GLFW_BINARY_DIR})
#  include_directories(${PROJECT_SOURCE_DIR}/glfw/include/)
#  include_directories(${PROJECT_SOURCE_DIR}/glm/)
#  target_link_libraries(${target} glfw ${GLEW_LIBRARY} ${GLFW_LIBRARIES} ) 

  #GLV
  find_package(GLV)
  target_link_libraries(${target} ${GLV_LIBRARY}) 
  include_directories(${GLV_INCLUDE_DIR})
endif()

#PROJECT INCLUDE AND LINK
include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/gfx/)
link_directories(${PROJECT_BINARY_DIR})



if(APPLE)
option(USE_CLANG "use clang" ON)
endif()
if(USE_CLANG)
  message("using clang compiler")
  SET(CMAKE_CXX_COMPILER "clang++")
  SET(CMAKE_C_COMPILER "clang")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -U__STRICT_ANSI__ -O3 -ftemplate-depth-1200 -Wno-switch -Wno-int-to-pointer-cast")# -L/Users/wolftype/code/libcxx/lib/")
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c++11 -U__STRICT_ANSI__ -nostdinc++ -O3 -ftemplate-depth-1200 -Wno-switch -Wno-int-to-pointer-cast")
else()
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -U__STRICT_ANSI__ -nostdinc++ -O3 -ftemplate-depth-1200 -Wno-switch -Wno-int-to-pointer-cast")
endif()


#DIRECTORIES
get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY LINK_DIRECTORIES)
foreach(d ${dirs})
  message(STATUS "dir='${d}'")
endforeach()







