/*
 * =====================================================================================
 *
 *       Filename:  xEGLWindow.cpp
 *
 *    Description:  Same as xGlutBuffer.cpp but on a Raspberry pi
 *
 *        Version:  1.0
 *        Created:  05/21/2014 18:30:09
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization:  
 *
 * =====================================================================================
 */

#include "util/gfx_egl.h"

#include "gfx_app.h"

#include "gfx_renderer.h"
#include "gfx_mbo.h"
#include "gfx_process.h"

using namespace gfx;
/*-----------------------------------------------------------------------------
 * example use of gfx_renderer with mesh buffer objects
 * Call Renderer::initGL() after initializing window context 
 *-----------------------------------------------------------------------------*/
struct MyApp : App<RPIContext> {

  MBO * mbo;
  float time = 0;

  MyApp(int w, int h, int argc, char ** argv) : App<RPIContext>(w,h,argc,argv) {
      init();
      process = new gfx::MotionTrace(_w,_h,this);  
  }

  void init(){
    mbo = new MBO( Mesh::Circle(1,10) );  
  }
  
  virtual void update(){
    mbo -> mesh.moveTo( sin(time) * scene.camera.lens.width()/2.0,0,0);
    mbo -> update();
  }
  
  virtual void onDraw(){
    time+=.015;      
    update();
    pipe.line(*mbo);
  }

  virtual void onFrame(){
    clear();
    (*process)();
    RPIContext::SwapBuffers();
  }
};


int main(int argc, char ** argv){
  MyApp app(21,14, argc, argv);
  app.start();
  return 0;
}

