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
#PROJECT INCLUDE AND LINK
include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/gfx/)
link_directories(${PROJECT_BINARY_DIR})


#DEPENDENCIES
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake_modules/")

if(${RPI})#question: how best to check for raspberry vs. linux vs. jetson etc here?

  find_package(GLES REQUIRED)
  find_package(EGL REQUIRED)
  include_directories(/usr/local/include)
  include_directories(/opt/vc/include)
  include_directories(/opt/vc/include/interface/vcos/pthreads)
  include_directories(/opt/vc/include/interface/vmcs_host/linux)
  link_directories(/opt/vc/lib)
  link_directories(/usr/lib)
  link_directories(/usr/local/lib)
  link_directories(/lib)

  set(libraries ${libraries} pthread bcm_host vchiq_arm vcos m stdc++ vcos rt ${EGL_LIBRARY} ${GLES_LIBRARY})

else()
  message ("CLANG")
  option(USE_CLANG "use clang" ON)
  find_package(OpenGL REQUIRED)
  find_package(Glew REQUIRED)
  find_package(GLUT REQUIRED)

  #GLV is optional
  find_package(GLV)
  include_directories(${GLV_INCLUDE_DIR})

  #GLFW  
#  set( GLFW_BUILD_EXAMPLES OFF CACHE BOOL "turn off build examples") 
#  add_subdirectory(glfw)
#  link_directories(${PROJECT_BINARY_DIR})
#  link_directories(${GLFW_BINARY_DIR})
#  include_directories(${PROJECT_SOURCE_DIR}/glfw/include/)
#  include_directories(${PROJECT_SOURCE_DIR}/glm/)
#  target_link_libraries(${target} glfw ${GLEW_LIBRARY} ${GLFW_LIBRARIES} ) 

  set(libraries ${libraries} ${GLUT_LIBRARY} ${OPENGL_LIBRARY} ${GLEW_LIBRARY} ) 
  set(libraries ${libraries} ${GLV_LIBRARY} ) 

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -U__STRICT_ANSI__ -O3 -ftemplate-depth-1200 -Wno-switch -Wno-int-to-pointer-cast")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c++11 -U__STRICT_ANSI__ -nostdinc++ -O3 -ftemplate-depth-1200 -Wno-switch -Wno-int-to-pointer-cast") 

endif()

#TARGETS
set( EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin )
file(GLOB examples examples/*.cpp)
foreach(filename ${examples})
  get_filename_component(target ${filename} NAME_WE)
  add_executable(${target} ${filename} )
  target_link_libraries(${target} ${libraries} )
endforeach()




#PRINT DIRECTORIES
get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY LINK_DIRECTORIES)
message("LINK DIRECTORES")
foreach(d ${dirs})
  message(STATUS "link dir='${d}'")
endforeach()
get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
message("INCLUDE DIRECTORES")
foreach(d ${dirs})
  message(STATUS "include dir='${d}'")
endforeach()







