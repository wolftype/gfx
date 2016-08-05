/*
 * =====================================================================================
 *
 *       Filename:  gl_texture.hpp
 *
 *    Description:  2D RGBA floats texture generation 
 *
 *    Note: include OpenGL first
 *
 *        Version:  1.0
 *        Created:  06/13/2014 16:23:42
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization:  pretty awesome
 *
 * =====================================================================================
 */


#ifndef  gl_texture_INC
#define  gl_texture_INC

#include "gfx_lib.h"

namespace gfx {

  struct Texture {

    //typedef TexType GL_UNSIGNED_BYTE;

    GLuint mId;
    int width, height;

    GLenum mType = GL_FLOAT;//UNSIGNED_BYTE;
    void type(GLenum t) { mType = t; }

    GLuint id() const { return mId; }
    
    Texture(int w, int h) : width(w), height(h) {
      init();    
    };

    void init(){

      printf("initialzing texture %d %d\n",width,height);
      /*-----------------------------------------------------------------------------
       *  Generate Texture and Bind it
       *-----------------------------------------------------------------------------*/
      glGenTextures(1, &mId);
      glBindTexture(GL_TEXTURE_2D, mId); 

      /*-----------------------------------------------------------------------------
       *  Allocate Memory on the GPU
       *-----------------------------------------------------------------------------*/
       // target | lod | internal_format | width | height | border | format | type | data
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, mType, NULL);
        
      /*-----------------------------------------------------------------------------
       *  Set Texture Parameters
       *-----------------------------------------------------------------------------*/
      // Set these parameters to avoid a black screen caused by improperly mipmapped textures
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);  //<-- or GL_LINEAR;
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  //<-- or GL_LINEAR;

      //optional . . .
      //
      //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

      /*-----------------------------------------------------------------------------
       *  Unbind texture
       *-----------------------------------------------------------------------------*/
       glBindTexture(GL_TEXTURE_2D, 0);

       GL::error("Generate Texture");
        
    }

    void bind() { glBindTexture(GL_TEXTURE_2D, mId); }
    void unbind() { glBindTexture(GL_TEXTURE_2D, 0); }


    void update(void * data){
       
       bind(); 
      /*-----------------------------------------------------------------------------
       *  Load data onto GPU
       *-----------------------------------------------------------------------------*/
      // target | lod | xoffset | yoffset | width | height | format | type | data
	    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, mType, data );
     //... glGenerateMipmap(GL_TEXTURE_2D);

      unbind();

    }
    
  };

/*-----------------------------------------------------------------------------
 *  OPENGL CUBEMAP TEXTURE 
 *-----------------------------------------------------------------------------*/
struct CubeMap {
  
  GLuint mID;
  int mWidth;//, mHeight;

  GLenum mType = GL_UNSIGNED_BYTE;
  void type(GLenum t) { mType = t; }

  GLuint id() const { return mID; }

  CubeMap(int w) : mWidth(w) { init(); }
  
  void init(){
    glEnable(GL_TEXTURE_CUBE_MAP);
    generate();
    alloc();
    setParam();
  }

  void generate(){
    glGenTextures(1, &mID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, mID);
  }

  void alloc(){
    for (int i = 0;i<6;++i){
      glTexImage2D (GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, //< target
                    0,                                //< lod
                    GL_RGBA,                          //< internal format
                    mWidth, mWidth, 0,               //< width, height, border (cube maps must be equal)
                    GL_RGBA,                          //< format of data
                    mType,                            //< data type (e.g. GL_UNSIGNED_BYTE)
                    NULL);                            //< no actual data yet
    }
  }

  void setParam(){
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); 
  }

  void bind(){
    glBindTexture(GL_TEXTURE_CUBE_MAP, mID);
  }
  void unbind() { glBindTexture(GL_TEXTURE_CUBE_MAP, 0); }
};

  

} //gfx::

#endif   /* ----- #ifndef gl_texture_INC  ----- */
