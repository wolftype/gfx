
#
# Try to find GLEW library and include path.
# Once done this will define
#
# GLEW_FOUND
# GLEW_INCLUDE_DIR
# GLEW_LIBRARY
# 

if ((NOT GLEW_INCLUDE_DIR) OR (NOT GLEW_LIBRARY))
  FIND_PATH( GLEW_INCLUDE_DIR GL/glew.h
    PATHS /usr/local/cuda-6.0/samples/common/inc/
    DOC "The directory where GL/glew.h resides")
  FIND_LIBRARY( GLEW_LIBRARY
    NAMES GLEW
    PATHS /usr/local/cuda-6.0/samples/common/lib/linux/armv7l/
    DOC "The directory where libGLEW resides")
endif ((NOT GLEW_INCLUDE_DIR) OR (NOT GLEW_LIBRARY))
