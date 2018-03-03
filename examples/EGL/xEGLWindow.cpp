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

#include "util/egl_window.h"
//#include "util/glut_window.hpp"

#include "gfx_app.h"

using namespace gfx;

/*-----------------------------------------------------------------------------
 * example use of gfx_renderer with mesh buffer objects
 *-----------------------------------------------------------------------------*/
struct MyApp : GFXApp<RPIContext> {

  MBO mbo;
  float time = 0;

  MyApp(int w, int h, string name) : GFXApp<RPIContext>(w,h,name) {
      // process = new gfx::MotionTrace(_w,_h,this);  
  }

  void setup(){
    mbo = MBO( Mesh::Circle(1,10) );
    mRenderer.immediate(false);  
  }
  
   void update(){
    mbo.mesh.moveTo( sin(time) * 3,0,0);//scene.camera.lens.width()/2.0,0,0);
    mbo.update();
  }
  
   void onDraw(){
    time+=.015;      
   // update();
    draw(mbo);
  }

};


int main(int argc, char ** argv){
  MyApp app(1600,1200, "test");
  app.start();
  return 0;
}

