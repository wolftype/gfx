/*
 * =====================================================================================
 *
 *       Filename:  gl_mesh.hpp
 *
 *       use: 
 *       
 *       MBO mbo;
 *       mbo.add( VertexAttribute(shaderID, 3, "position", sizeof(Vertex), 0) );
 *       mbo.buffer( Mesh...);
 *
 *    Description:  encapsulation of vertex array object, vertex buffer objects, and vertex array pointers
 *
 *    NOTE: assumes shader defines attributes position, color, normal and textureCoordinate
 *
 *    assumes we will be updating vertex information frequently, but not index information
 *
 *        Version:  1.0
 *        Created:  06/13/2014 16:22:02
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef  gfx_mbo_INC
#define  gfx_mbo_INC

#include <vector>
#include "gfx_macros.h"

namespace gfx{

struct VertexAttribute {
  
  GLuint id;
  GLuint mSize;
  GLenum mType;
  GLsizei mStride;
  size_t mOffset;

  VertexAttribute(GLuint shaderID, GLuint size, string name, GLsizei stride, size_t offset) :
  mSize(size), mStride(stride), mOffset(offset) {
    get(shaderID, name);
  }

  void get(GLuint shaderID, string name){ id=glGetAttribLocation(shaderID, name.c_str()); }
  void enable() { if (id!=-1) glEnableVertexAttribArray(id); }
  void pointer(){ 
    if (id!=-1) {
      glVertexAttribPointer(id, mSize, GL_FLOAT, GL_FALSE, mStride, (void*) mOffset );
    } 
  }

};

struct MBO{

  //ID of Vertex Attribute
  GLuint positionID, normalID, colorID, textureCoordinateID;          //<-- Attribute IDS
 
  vector<VertexAttribute> attrib;
  //A buffer ID and elementID
  GLuint bufferID, elementID;
  //An array object ID
  GLuint arrayID;

  MBO(){}

  MBO& add(const VertexAttribute& v){
    attrib.push_back(v); 
  }
  
  template<class VERTEX, class ELEM>
  void buffer(const vector<VERTEX>& vertices, const vector<ELEM>& indices){
    
    /*-----------------------------------------------------------------------------
     *  CREATE THE VERTEX ARRAY OBJECT
     *-----------------------------------------------------------------------------*/
    GENVERTEXARRAYS(1, &arrayID);
    BINDVERTEXARRAY(arrayID);

    /*-----------------------------------------------------------------------------
     *  CREATE THE VERTEX BUFFER OBJECT
     *-----------------------------------------------------------------------------*/
    // Generate one buffer
    glGenBuffers(1, &bufferID);
    glBindBuffer( GL_ARRAY_BUFFER, bufferID );
    glBufferData( GL_ARRAY_BUFFER, vertices.size() * sizeof(VERTEX), &(vertices[0]), GL_DYNAMIC_DRAW ); //<-- Prep for frequent updates

     /*-----------------------------------------------------------------------------
     *  CREATE THE ELEMENT ARRAY BUFFER OBJECT
     *-----------------------------------------------------------------------------*/
    glGenBuffers(1, &elementID);
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, elementID );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(ELEM), &(indices[0]), GL_STATIC_DRAW ); //<-- Update infrequently, if ever

    
    /*-----------------------------------------------------------------------------
     *  BIND ATTRIBUTES
     *-----------------------------------------------------------------------------*/
    for (int i=0;i<attrib.size();++i) {
      attrib[i].enable();
      attrib[i].pointer();
    }

    // Unbind Everything (NOTE: unbind the vertex array object first)
    BINDVERTEXARRAY(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); 
  }

  template<class VERTEX>
  void subBuffer(const vector<VERTEX>& vertices){
    bind();
    glBufferSubData( GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(VERTEX), vertices.data() );
    unbind();
  }

  void bind(){ BINDVERTEXARRAY(arrayID); }
  void unbind() { BINDVERTEXARRAY(0); }

  template<class VERTEX>
  void drawArrays( GLuint mode, const vector<VERTEX>& vertices ){ glDrawArrays( mode, 0, vertices.size() ); }
  template<class ELEM>
  void drawElements( GLuint mode, const vector<ELEM>& indices){ glDrawElements( mode, indices.size(), GL_UNSIGNED_SHORT, 0 ); }


  /* void draw(){ */
  /*   bind(); */
  /*     bElements ? drawElements : drawArrays */
  /*   unbind(); */
  /* } */
  
};

} //gfx::


#endif   /* ----- #ifndef gl_mesh_INC  ----- */
