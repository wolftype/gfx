
#
# Try to find GLES library and include path.
# Once done this will define
#
# GLES_FOUND
# GLES_INCLUDE_DIR
# GLES_LIBRARY
# 

if ((NOT GLES_INCLUDE_DIR) OR (NOT GLES_LIBRARY))
  FIND_PATH( GLES_INCLUDE_DIR 
    NAMES GLES2/gl2.h OpenGLES/ES2/gl.h
    PATHS $ENV{HOME}/pi-root/opt/vc/include $ENV{HOME}/usr/
    DOC "The directory where GLES/gl.h resides")
  FIND_LIBRARY( GLES_LIBRARY
    NAMES GLESv2
    PATHS $ENV{HOME}/pi-root/opt/vc/lib $ENV{HOME}/usr/lib/arm-linux-gnueabihf/mesa-egl/ 
    DOC "The directory where libGLES resides")
endif ((NOT GLES_INCLUDE_DIR) OR (NOT GLES_LIBRARY))
