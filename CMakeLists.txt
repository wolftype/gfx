#CMAKE FOR GLUT WINDOWING OR EGL WINDOWING + GFX GL OR GLES RENDERING DEMOS
#for cross-compiling to raspberry pi or jetson etc use -DCMAKE_TOOLCHAIN_FILE=path/to/file (e.g. toolchains/rpi)

#VERSION
cmake_minimum_required(VERSION 2.8)

#POLICIES
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 OLD)
endif(COMMAND cmake_policy)

#message(${CMAKE_SYSTEM_NAME})
#PROJECT
project(GFX)

#PROJECT INCLUDE AND LINK
include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/gfx/)
link_directories(${PROJECT_BINARY_DIR})

#DEPENDENCIES
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/modules/")

#RASPBERRY FLAG IS SET BY TOOLCHAIN FILE FOR CONDITIONAL INCLUDES
if(${RASPBERRY})
  
  message ("**********CONFIGURING GFX LIBRARY BUILD FOR RASPBERRY PI************")
  message ("**********root set to " ${CMAKE_FIND_ROOT_PATH} "************")

  find_package(GLES REQUIRED)
  find_package(EGL REQUIRED)
 
  include_directories(${CMAKE_FIND_ROOT_PATH}/usr/include) 
  include_directories(${CMAKE_FIND_ROOT_PATH}/usr/local/include)
  include_directories(${CMAKE_FIND_ROOT_PATH}/opt/vc/include)
  include_directories(${CMAKE_FIND_ROOT_PATH}/opt/vc/include/interface/vcos/pthreads)
  include_directories(${CMAKE_FIND_ROOT_PATH}/opt/vc/include/interface/vmcs_host/linux)
  
  link_directories(/opt/vc/lib)
  link_directories(/usr/lib)
  link_directories(/usr/local/lib)
  link_directories(/lib)

  set(libraries ${libraries} pthread bcm_host vchiq_arm vcos m stdc++ vcos rt ${EGL_LIBRARY} ${GLES_LIBRARY})

  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D __raspberry__ -D __LINUX_ALSA__ -std=c++0x")
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D __raspberry__ -D __LINUX_ALSA__ -std=c++0x")

elseif(${APPLE})

  message ("**********CONFIGURING GFX LIBRARY BUILD FOR OSX************")

  option(USE_CLANG "use clang" ON)
  
  find_package(OpenGL REQUIRED)
  find_package(Glew REQUIRED)
  find_package(GLUT REQUIRED)

  include_directories(${OPENGL_INCLUDE_DIR})
  include_directories(${GLEW_INCLUDE_DIR})
  include_directories(${GLUT_INCLUDE_DIR})

  #GLV should be optional
  find_package(GLV REQUIRED)
  message ("GLV LIBRARY FOUND AT:" ${GLV_LIBRARY})
  message ("GLV HEADERS FOUND AT:" ${GLV_INCLUDE_DIR})
  
 # if(${GLV_FOUND})
  include_directories(${GLV_INCLUDE_DIR})
  set(libraries ${libraries} ${GLV_LIBRARY} )
 # endif()
  include_directories(gl2ps)

  #GLFW  
#  set( GLFW_BUILD_EXAMPLES OFF CACHE BOOL "turn off build examples") 
#  add_subdirectory(glfw)
#  link_directories(${PROJECT_BINARY_DIR})
#  link_directories(${GLFW_BINARY_DIR})
#  include_directories(${PROJECT_SOURCE_DIR}/glfw/include/)
#  include_directories(${PROJECT_SOURCE_DIR}/glm/)
#  target_link_libraries(${target} glfw ${GLEW_LIBRARY} ${GLFW_LIBRARIES} ) 

  #small lib just compiles gl2ps 
  add_library(gfx gl2ps/gl2ps.c)
  
  set(libraries ${libraries} ${GLUT_LIBRARY} ${OPENGL_LIBRARY} ${GLEW_LIBRARY} gfx) 

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -U__STRICT_ANSI__ -O3 -ftemplate-depth-1200 -Wno-switch -Wno-int-to-pointer-cast -Wno-deprecated-declarations")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -U__STRICT_ANSI__ -nostdinc++ -O3 -ftemplate-depth-1200 -Wno-switch -Wno-int-to-pointer-cast") 

  #TARGETS
  set( EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin )
  file(GLOB examples examples/GL/*.cpp)
  foreach(filename ${examples})
    get_filename_component(target ${filename} NAME_WE)
    add_executable(${target} ${filename} )
    target_link_libraries(${target} ${libraries} )
  endforeach()

elseif(${CMAKE_SYSTEM_NAME} MATCHES Linux)

  message ("**********CONFIGURING GFX LIBRARY BUILD FOR LOCAL BUILD MACHINE************")

  option(USE_CLANG "use clang" ON)
  
  find_package(OpenGL REQUIRED)
  find_package(GLEW REQUIRED)
  find_package(GLUT REQUIRED)

  #GLV should be optional
  find_package(GLV REQUIRED)
  message ("GLV LIBRARY FOUND AT:" ${GLV_LIBRARY})
  message ("GLV HEADERS FOUND AT:" ${GLV_INCLUDE_DIR})
  
 # if(${GLV_FOUND})
  include_directories(${GLV_INCLUDE_DIR})
  set(libraries ${libraries} ${GLV_LIBRARY} )
 # endif()
  include_directories(gl2ps)

  #GLFW  
#  set( GLFW_BUILD_EXAMPLES OFF CACHE BOOL "turn off build examples") 
#  add_subdirectory(glfw)
#  link_directories(${PROJECT_BINARY_DIR})
#  link_directories(${GLFW_BINARY_DIR})
#  include_directories(${PROJECT_SOURCE_DIR}/glfw/include/)
#  include_directories(${PROJECT_SOURCE_DIR}/glm/)
#  target_link_libraries(${target} glfw ${GLEW_LIBRARY} ${GLFW_LIBRARIES} ) 

  #small lib just compiles gl2ps 
  add_library(gfx gl2ps/gl2ps.c)
  
  set(libraries ${libraries} ${GLUT_LIBRARY} ${OPENGL_LIBRARY} ${GLEW_LIBRARY} gfx) 

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -U__STRICT_ANSI__ -O3 -ftemplate-depth-1200 -Wno-switch -Wno-int-to-pointer-cast -Wno-deprecated-declarations")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -U__STRICT_ANSI__ -nostdinc++ -O3 -ftemplate-depth-1200 -Wno-switch -Wno-int-to-pointer-cast") 

endif()


#PRINT DIRECTORIES
get_property(link_dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY LINK_DIRECTORIES)
message("GFX LINK DIRECTORES")
foreach(d ${link_dirs})
  message(STATUS "link dir='${d}'")
endforeach()
get_property(inc_dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
message("GFX INCLUDE DIRECTORES")
foreach(d ${inc_dirs})
  message(STATUS "include dir='${d}'")
endforeach()

#SET TO A GLOBAL PROPERTY OF THIS SUBDIRECTORY
set_property(GLOBAL PROPERTY GFX_LIBRARIES "${libraries}")
set_property(GLOBAL PROPERTY GFX_INCLUDE_DIR "${inc_dirs}")
set_property(GLOBAL PROPERTY GFX_LINK_DIR "${link_dirs}")




