#CMAKE FOR GLUT WINDOWING OR EGL WINDOWING + GFX GL OR GLES RENDERING DEMOS
#for cross-compiling to raspberry pi or jetson etc use -DCMAKE_TOOLCHAIN_FILE=path/to/file (e.g. toolchains/rpi)

#VERSION
cmake_minimum_required(VERSION 2.8)

#POLICIES
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 OLD)
endif(COMMAND cmake_policy)

#PROJECT
project(GFX)

MESSAGE ("\n\n******SYSTEM INFORMATION******")
MESSAGE( "System Name: " ${CMAKE_SYSTEM_NAME} )
MESSAGE( "System Processor: " ${CMAKE_SYSTEM_PROCESSOR} )
MESSAGE( "*******************************\n\n")

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  SET( LINUX 1)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  SET( APPLE 1 )
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  SET( WINDOWS 1)
endif()

#default release type if none specified
if (NOT CMAKE_BUILD_TYPE)
  set (CMAKE_BUILD_TYPE RelWithDebInfo)
endif ()

if(CMAKE_BUILD_TYPE MATCHES Debug)
  set(BUILD_TYPE_FLAGS "-g -O0")
elseif(CMAKE_BUILD_TYPE MATCHES Release)
  set(BUILD_TYPE_FLAGS "-g0 -O3")
elseif(CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
  set(BUILD_TYPE_FLAGS "-g -O3")
else()
  message(FATAL_ERROR "invalid build type or failure to set a build type.  specify Release, Debug, or RelWithDebInfo")
endif()

message ("BUILD EXAMPLES: " ${BUILD_EXAMPLES} )
message ("USE GLFW: " ${GFX_USE_GLFW} )

#PROJECT INCLUDE AND LINK
include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/gfx/)
link_directories(${PROJECT_BINARY_DIR})

#DEPENDENCIES
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/modules/")

#CUSTOM TARGETS FOR IDES
FILE(GLOB HeaderFiles "${PROJECT_SOURCE_DIR}/gfx/*")
add_custom_target(GFX_HEADERS SOURCES ${HeaderFiles})
FILE(GLOB UtilFiles "${PROJECT_SOURCE_DIR}/gfx/util/*")
add_custom_target(GFX_UTIL SOURCES ${UtilFiles})

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

  message ("\n**********CONFIGURING GFX LIBRARY BUILD FOR OSX************\n")

  option(USE_CLANG "use clang" ON)

  find_package(OpenGL REQUIRED)
  find_package(GLUT REQUIRED)
  find_package(GLEW REQUIRED)

  if(${GLEW_FOUND})
    message("GLEW FOUND:")
    message( "${GLEW_INCLUDE_DIR}" )
    message( "${GLEW_LIBRARY}" )
  endif()
  if(${GLUT_FOUND})
    message("GLUT FOUND:")
    message( "${GLUT_INCLUDE_DIR}" )
    message( "${GLUT_LIBRARY}" )
  endif()

  include_directories(${OPENGL_INCLUDE_DIR})
  include_directories(${GLEW_INCLUDE_DIR})
  include_directories(${GLUT_INCLUDE_DIR})

 add_subdirectory(GLV)
 include_directories(gl2ps)

  #GLFW
  #  if (${GFX_USE_GLFW})
#    set( GLFW_BUILD_EXAMPLES OFF CACHE BOOL "turn off build examples")
#  add_subdirectory(glfw)
#  link_directories(${PROJECT_BINARY_DIR})
#  link_directories(${GLFW_BINARY_DIR})
#  include_directories(${PROJECT_SOURCE_DIR}/glfw/include/)
#  include_directories(${PROJECT_SOURCE_DIR}/glm/)
#  target_link_libraries(${target} glfw ${GLEW_LIBRARY} ${GLFW_LIBRARIES} )

  #small lib just compiles gl2ps
  add_library(gfx gl2ps/gl2ps.c)

  set(gfx_library gfx)
  set(libraries ${libraries} ${GLUT_LIBRARY} ${OPENGL_LIBRARY} ${GLEW_LIBRARY} ${gfx_library})

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -U__STRICT_ANSI__ -O3 -ftemplate-depth-1200 -Wno-switch -Wno-int-to-pointer-cast -Wno-deprecated-declarations")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -U__STRICT_ANSI__ -nostdinc++ -O3 -ftemplate-depth-1200 -Wno-switch -Wno-int-to-pointer-cast")

  #TARGETS
  set( EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin )
  file(GLOB examples examples/GL/*.cpp)
  foreach(filename ${examples})
    message (${filename})
    get_filename_component(target ${filename} NAME_WE)
    if(${BUILD_EXAMPLES})
      add_executable(${target} ${filename} )
    else ()
      add_executable(${target} EXCLUDE_FROM_ALL ${filename} )
    endif()
    target_link_libraries(${target} ${libraries} )
  endforeach()

elseif(${CMAKE_SYSTEM_NAME} MATCHES Linux)

  message ("**********CONFIGURING GFX LIBRARY BUILD FOR LOCAL BUILD MACHINE************")

  option(USE_CLANG "use clang" ON)

  find_package(OpenGL REQUIRED)

  #todo, these should not be required
  find_package(GLUT REQUIRED)
  find_package(GLEW REQUIRED)

  #GLV should be optional
  add_subdirectory(GLV)
  #  find_package(GLV OPTIONAL)
  #  message ("GLV LIBRARY FOUND AT:" ${GLV_LIBRARY})
  #  message ("GLV HEADERS FOUND AT:" ${GLV_INCLUDE_DIR})
  #
  # if(${GLV_FOUND})
  #  include_directories(${GLV_INCLUDE_DIR})
  #  set(libraries ${libraries} ${GLV_LIBRARY} )
  # endif()
  #  include_directories(gl2ps)

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

  set(gfx_library gfx)
  set(libraries ${libraries} ${GLUT_LIBRARY} ${OPENGL_LIBRARY} ${GLEW_LIBRARY} ${gfx_library})

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -U__STRICT_ANSI__ -O3 -ftemplate-depth-1200 -Wno-switch -Wno-int-to-pointer-cast -Wno-deprecated-declarations")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -U__STRICT_ANSI__ -nostdinc++ -O3 -ftemplate-depth-1200 -Wno-switch -Wno-int-to-pointer-cast")

elseif(${WINDOWS})

  message ("\n**********CONFIGURING GFX LIBRARY BUILD FOR WINDOWS MACHINE ************\n")

  find_package(OpenGL REQUIRED)

  set( GLFW_BUILD_EXAMPLES OFF CACHE BOOL "turn off build examples")
  add_subdirectory(glfw)

  link_directories(${PROJECT_BINARY_DIR})
  link_directories(${GLFW_BINARY_DIR})
  link_directories(${PROJECT_SOURCE_DIR}/win/glew/lib/)

  INCLUDE_DIRECTORIES(gl2ps)
  INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/win/glew/include/)
  INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR})
  INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/glfw/include/)

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -U__STRICT_ANSI__ -O3 -ftemplate-depth-1200")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -U__STRICT_ANSI__ -nostdinc++ -O3 -ftemplate-depth-1200")

  FILE(GLOB ExampleFiles "${PROJECT_SOURCE_DIR}/examples/GL/*")
  foreach( srcfile ${ExampleFiles} )
    get_filename_component(target ${srcfile} NAME_WE)
    add_executable(${target} EXCLUDE_FROM_ALL ${srcfile} )
	target_link_libraries(${target} glfw glew32s ${GLFW_LIBRARIES} )
  endforeach( srcfile ${ExampleFiles} )

endif()

get_property(link_dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY LINK_DIRECTORIES)
get_property(inc_dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)

#SET TO A GLOBAL PROPERTY FOR WHEN ADDING GFX AS A SUBDIRECTORY
set_property(GLOBAL PROPERTY GFX_LIBRARIES "${libraries}")
set_property(GLOBAL PROPERTY GFX_INCLUDE_DIR "${inc_dirs}")
set_property(GLOBAL PROPERTY GFX_LINK_DIR "${link_dirs}")

foreach(lib ${gfx_libraries})
  message(STATUS "libraries='${lib}'")
endforeach()

install(TARGETS ${gfx_library}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

#INSTALL HEADERS
install(DIRECTORY gfx DESTINATION include)

#if(${GLV_FOUND})
#message ("GLV ADDED TO INSTALL")
##GLV does not come with its own install mechanism
#install(DIRECTORY GLV DESTINATION include)
#install(TARGETS ${GLV_LIBRARY}
#  RUNTIME DESTINATION bin
#  LIBRARY DESTINATION lib
#  ARCHIVE DESTINATION lib)
#endif()

#PRINT DIRECTORIES
#message("GFX LINK DIRECTORES")
#foreach(d ${link_dirs})
#  message(STATUS "link dir='${d}'")
#endforeach()

#message("GFX INCLUDE DIRECTORES")
#foreach(d ${inc_dirs})
#  message(STATUS "include dir='${d}'")
#endforeach()


