/*
 * =====================================================================================
 *
 *       Filename:  gfx_fbo.h
 *
 *    Description:  framebuffer objects
 *
 *        Version:  1.0
 *        Created:  01/30/2015 19:26:16
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization: pretty awesome 
 *
 * =====================================================================================
 */

#ifndef  gfx_fbo_INC
#define  gfx_fbo_INC

#include "gfx_texture.h"

using namespace gfx;

struct FBO{

   GLuint mID;
   GLuint mDepthID;
   int width, height;

   bool bDepth=true;

   void depth(bool d) { bDepth = d; }

   void set(int w, int h){
      width = w; height=h;
   }
  
   void init(){
     generate();
     if(bDepth) generateDepthBuffer();
     checkStatus();
   }

    void checkStatus(){
      GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
      if (status != GL_FRAMEBUFFER_COMPLETE){
          cout <<"****************\nframebuffer incomplete\n*****************" << endl;
#ifndef GFX_USE_GLES
          switch(status){
            case GL_FRAMEBUFFER_UNDEFINED: cout<< "undefined"<<endl; break;
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: cout << "incomplete attachment" << endl; break; 
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: cout << "missing attachment" << endl;break;
            case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:cout << "incomplete draw buffer"<< endl;break;
            case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:cout << "incomplete read buffer"<< endl;break;
            case GL_FRAMEBUFFER_UNSUPPORTED:cout <<"unsupported"<<endl;break;
            case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:cout<<"incomplete multisample"<<endl;break;
            case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:cout<<"incomplete layer targets"<<endl;break;
            default: GL::error("framebuffer check");
          }
#endif
        }
    }

   void clear(){
     glViewport(0, 0, width, height ); 
     glClearColor(0,0,0,0);
     glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   }

   void clearDepth(){
     glClear(GL_DEPTH_BUFFER_BIT);
   }   
   
   //could change attachment point or target (e.g. to GL_TEXTURE_CUBE_MAP)
   void attach(const Texture& tex, GLenum attachment = GL_COLOR_ATTACHMENT0, GLenum target = GL_TEXTURE_2D ){
     bind();
     glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, target, tex.id(), 0);
     unbind();
     checkStatus();
   }
 
    //could change attachment point or target (e.g. to GL_TEXTURE_CUBE_MAP)
    void attach(const CubeMap& tex, GLenum attachment = GL_COLOR_ATTACHMENT0, GLenum target = GL_TEXTURE_CUBE_MAP_POSITIVE_X ){
     bind();
     glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, target, tex.id(), 0);
     unbind();
     checkStatus();
   }
   
   void generate(){
     glGenFramebuffers(1,&mID);
   }

   void generateDepthBuffer(){
    bind();
      glGenRenderbuffers(1, &mDepthID);
      glBindRenderbuffer(GL_RENDERBUFFER, mDepthID);
      glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
      glBindRenderbuffer(GL_RENDERBUFFER, 0);
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthID);
    unbind();
   }

   void renderbuffer(){
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthID);
   }
   
   void bind(){
     glBindFramebuffer(GL_FRAMEBUFFER,mID);
     if(bDepth) glEnable(GL_DEPTH_TEST);
     else glDisable(GL_DEPTH_TEST);
   }
   
   void unbind(){
     glEnable(GL_DEPTH_TEST);
     glBindFramebuffer(GL_FRAMEBUFFER,0);
   }

   ~FBO(){
     glDeleteFramebuffers(1, &mID);
   }

};


/// handy fbo template with textures
//template <class T>
//struct FBO {
//
//};

#endif   /* ----- #ifndef gfx_fbo_INC  ----- */
