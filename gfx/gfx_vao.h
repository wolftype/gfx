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
    GENVERTEXARRAYS(1,&id); //<-- This macro calls appropriate glGenVertexArray function (or glGenVertexArrayAPPLE, etc.)
  }

  void bind() const {
    BINDVERTEXARRAY(id);    //<-- This macro calls appropriate glBindVertexArray function
  }

  void unbind() const {
    BINDVERTEXARRAY(0);
  }

};

} //gfx::


#endif   /* ----- #ifndef gfx_vao_INC  ----- */
