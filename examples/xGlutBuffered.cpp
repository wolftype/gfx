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

#include "util/GlutWindow.h"
#include "gfx_app.h"

#include "gfx_renderer.h"
#include "gfx_mbo.h"
#include "gfx_process.h"

using namespace gfx;
/*-----------------------------------------------------------------------------
 * example use of gfx_renderer with mesh buffer objects
 * Call Re:nderer::initGL() after initializing window context 
 *-----------------------------------------------------------------------------*/
struct MyApp : App<Window> {

  MBO * mbo;
  float time = 0;

  MyApp(int w, int h, int argc, char ** argv) : App<Window>(w,h,argc,argv) {
      init();
  }

  void init(){
    mbo = new MBO( Mesh::Circle(1,10) );  
    scene.camera.pos(0,0,5);
  }
  
  virtual void update(){
    mbo -> mesh.moveTo( sin(time) * scene.camera.lens.width()/2.0,0,0);
    mbo -> update();
  }

  virtual void onDraw(){
    time+=.005;    
    update();
    pipe.line(*mbo);
  }

};

int main(int argc, char ** argv){
  MyApp app(500,500, argc, argv);
  app.start();
  return 0;
}
