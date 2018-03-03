
#
# Try to find GLV library and include path.
# Once done this will define
#
# GLV_FOUND
# GLV_INCLUDE_DIR
# GLV_LIBRARY
# 

if ((NOT GLV_INCLUDE_DIR) OR (NOT GLV_LIBRARY))
  FIND_PATH( GLV_INCLUDE_DIR GLV/glv.h
    PATHS ../GLV/ ../../GLV/ ../../../GLV/ ../glv/ ../../glv ../../../glv
    DOC "The directory where GLV/glv.h resides")
  FIND_LIBRARY( GLV_LIBRARY
    NAMES GLV
    PATHS ../GLV/build/lib/ ../../GLV/build/lib/ ../../../GLV/build/lib/  ../glv/build/lib/ ../../glv/build/lib/ ../../../glv/build/lib/
    DOC "The directory where libGLV resides")
endif ((NOT GLV_INCLUDE_DIR) OR (NOT GLV_LIBRARY))
