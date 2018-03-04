/*
 * =====================================================================================
 *
 *       Filename:  gl_macros.h
 *
 *    Description:  some useful #defines like PI and STRINGIFY and GLSL
 *                  include after including glew.h
 *
 *        Version:  1.0
 *        Created:  06/18/2014 17:42:36
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization:  pretty awesome
 *
 * =====================================================================================
 */

#ifndef gl_macros_INC
#define gl_macros_INC

#include <math.h>

namespace gfx {

#ifndef PI
#define PI (float) 3.14159265359
#endif

#ifndef STRINGIFY
#define STRINGIFY(A) #A
#endif

//SAME AS STRINGIFY, but with a version preprocessor command
#ifndef GLSL
#define GLSL(version, A) "#version " #version "\n" #A
#endif

#ifndef RAND
#define RAND (1.0 * rand () / (RAND_MAX))
#endif
}

#endif /* ----- #ifndef gl_macros_INC  ----- */
