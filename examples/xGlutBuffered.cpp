/*
 * =====================================================================================
 *
 *       Filename:  xGlut.h
 *
 *    Description:  glut windowing
 *
 *        Version:  1.0
 *        Created:  05/19/2014 19:28:08
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization:  
 *
 * =====================================================================================
 */

#include "util/glut_window.hpp"
#include "gfx_app.h"
#include "gfx_mbo.h"
#include "gfx_tprocess.h"

using namespace gfx;

/*-----------------------------------------------------------------------------
 * example use of rendering with mesh buffer objects
 * Calls Renderer::init() after initializing window context 
 *-----------------------------------------------------------------------------*/
struct MyApp : GFXApp<GlutContext> {

  MBO * mbo;
  float time = 0;

  GFXRenderer renderer;
  Slab slab;
  unsigned char * tdata;

  MyApp(int w, int h, int argc, char ** argv) : 
  GFXApp<GlutContext>(w,h,argc,argv),
  renderer(w,h,this),
  slab(w,h)
  { 
      scene.immediate(false);         //<-- don't use immediate mode
      renderer.init();
      renderer.scene( &(this->scene) ); 
      setup();

  }

  void setup(){
    mbo = new MBO( Mesh::Circle() );
    
    tdata = new unsigned char[window().width()*window().height()*4];
  
    for (int i=0;i<window().width();++i){
      for (int j=0;j<window().height();++j){
        for (int k=0;k<4;++k){
         tdata[i*window().height()*4 + j*4 + k] = i+j-(k*j);
        }
      }
    }
    slab.texture -> data(tdata); 
    slab.texture -> update();
  }
  
  virtual void update(){
    static float counter = 0;
    counter += .01;

    mbo -> mesh.moveTo( sin(counter) * scene.camera.lens.width()/2.0,0,0);
    mbo -> update();
  }

  virtual void onDraw(){
    GFX::Render(*mbo, renderer.vatt);
   // slab();
  }

  virtual void onFrame(){
     clear();
     //update(); 
     renderer(); //<-- Renderer binds shader and calls this->onRender() method;
     scene.step();
  }

};

int main(int argc, char ** argv){
  MyApp app(800,400, argc, argv);
  app.start();
  return 0;
}
