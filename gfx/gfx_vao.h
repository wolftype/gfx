/*
 * =====================================================================================
 *
 *       Filename:  gfx_vao.h
 *
 *    Description:  vertex array object: stores all info, simplifies point into gpu
 *                  see gfx_macros.h for implementation of glGenVertexArray and glBindVertexArray
 *
 *        Version:  1.0
 *        Created:  10/13/2014 16:15:24
 *       Revision:  none
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *
 * =====================================================================================
 */


#ifndef  gfx_vao_INC
#define  gfx_vao_INC

#include "gfx_lib.h"

namespace gfx {

struct VAO  {

  GLuint id;

  VAO(){
    generate();
    bind();
  }

  void generate(){
#ifndef GFX_USE_GLES
    GENVERTEXARRAYS(1,&id); //<-- This macro calls appropriate glGenVertexArray function (or glGenVertexArrayAPPLE, etc.)
#endif
  }

  void bind() const {
#ifndef GFX_USE_GLES    
    BINDVERTEXARRAY(id);    //<-- This macro calls appropriate glBindVertexArray function
#endif
  }

  void unbind() const {
#ifndef GFX_USE_GLES
    BINDVERTEXARRAY(0);
#endif
  }


};

} //gfx::


#endif   /* ----- #ifndef gfx_vao_INC  ----- */
