#
# Try to find EGL library and include path.
# Once done this will define
#
# EGL_FOUND
# EGL_INCLUDE_DIR
# EGL_LIBRARY
# 

if ((NOT EGL_INCLUDE_DIR) OR (NOT EGL_LIBRARY))
  FIND_PATH( EGL_INCLUDE_DIR EGL/egl.h
    $ENV{HOME}/pi-root/opt/vc/include
    DOC "The directory where EGL/egl.h resides")
  FIND_LIBRARY( EGL_LIBRARY
    NAMES EGL
    PATHS $ENV{HOME}/pi-root/opt/vc/lib 
    DOC "The directory where libEGL resides")
endif ((NOT EGL_INCLUDE_DIR) OR (NOT EGL_LIBRARY))
