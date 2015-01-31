/*
 * =====================================================================================
 *
 *       Filename:  xRenderToTexture.cpp
 *
 *    Description:  Render to a Texture and then Draw Texture on Screen
 *
 *        Version:  1.0
 *        Created:  01/28/2015 16:28:28
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization:  pretty awesome 
 *
 * =====================================================================================
 */

#include "gfx_app.h"
#include "util/glut_window.hpp"

using namespace gfx;

struct R2T{
   GLuint mID;
   GLuint mDepthID;
  
   Texture * textureA;
   Texture * textureB;

   int width = 100;
   int height = 100;

   void set(int w, int h){ width=w; height=h; }

   void init(){
     generate();
     generateDepthBuffer();

     textureA = new Texture(width,height);
     textureB = new Texture(width,height);
   }
   
   void clear(){
     glViewport(0, 0, width, height ); 
     glClearColor(0,0,0,0);
     glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   }

   void attach(){
     bind();
     glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureA->idx(), 0);
     unbind();
   }
   
   void generate(){
     glGenFramebuffers(1,&mID);
   }

   void generateDepthBuffer(){
    bind();
      glGenRenderbuffers(1, &mDepthID);
      glBindRenderbuffer(GL_RENDERBUFFER, mDepthID);
      glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthID);
    unbind();
   }
   
   void bind(){
     glBindFramebuffer(GL_FRAMEBUFFER,mID);
   }
   
   void unbind(){
     glBindFramebuffer(GL_FRAMEBUFFER,0);
   }

   void swap() { Texture * tmp = textureA; textureA = textureB; textureB = tmp;  };
};

struct MyApp : GFXApp<GlutContext> {

 MBO mbo;
 R2T r2t;

 Slab slab;

 virtual void setup(){

    mbo = Mesh::Sphere();
    mRenderer.immediate(false); 

    r2t.set(io().viewdata.w,io().viewdata.h );
    
    r2t.init();
    slab.init();

//    float * cval = new float[slab.width*slab.height*4];
    unsigned char * cval = new  unsigned char [slab.width*slab.height*4];
        
    for (int i =0;i<slab.width;++i){
      for (int j=0;j<slab.height;++j){
          cval[i*slab.height*4+j*4] = 255*(float)i/slab.width;
          cval[i*slab.height*4+j*4+1] = 255*(float)j/slab.height;
          cval[i*slab.height*4+j*4+2] = 255*(float)j/i;
          cval[i*slab.height*4+j*4+3] = 128; 
          
      }
    }

   // slab.texture->data(cval);
   // slab.texture->update();
    slab.texture = r2t.textureA;
 }

 virtual void onFrame(){
    //r2t.enter();
    r2t.attach();
    r2t.bind();
    r2t.clear();
      
      GL::enablePreset();

      scene.push( mRenderer.immediate() ); ///< push matrices

        mRenderer.enter();                      ///< bind shader and update shader uniforms
          onDraw();                         ///< call this->onRender() method (defaults to this->onDraw())  
        mRenderer.exit();                       ///< unbind shader
    
      scene.pop( mRenderer.immediate() );  ///< pop matrices
    
      scene.step();                            ///< update camera physics

    r2t.unbind();
    clear();
     GL::disablePreset();

    mContext.setViewport();

    slab.onRender();
    r2t.swap();

 }

 /* virtual void onRender(){ */
   
 /*    printf("not\n"); */ 
 /*    r2t.enter(); */

 /*     onDraw(); */
   
 /*    r2t.exit(); */

 /*    mContext.setViewport(); */

 /*    slab.texture = r2t.textureB; */
 /*    slab.onRender(); */
 /*    r2t.swap(); */

 /* } */

 virtual void onDraw(){

     draw(mbo,1,0,0);

 }


};


int main(){

  MyApp app;
  app.start();
  return 0;
}
