
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
    $ENV{HOME}/pi-root/opt/vc/include
    DOC "The directory where GLES/egl.h resides")
  FIND_LIBRARY( GLES_LIBRARY
    NAMES GLESv2
    PATHS $ENV{HOME}/pi-root/opt/vc/lib 
    DOC "The directory where libGLES resides")
endif ((NOT GLES_INCLUDE_DIR) OR (NOT GLES_LIBRARY))
