/*
 * =====================================================================================
 *
 *       Filename:  xGlut.h
 *
 *    Description:  glut windowing advanced
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
#include "gfx_renderer.h"
#include "gfx_mbo.h"
#include "gfx_process.h"

using namespace gfx;
/*-----------------------------------------------------------------------------
 * example use of gfx_renderer with mesh buffer objects
 * Call Renderer::initGL() after initializing window context 
 *-----------------------------------------------------------------------------*/
template<class CONTEXT>
struct App : Renderer {

  MBO * mbo;
  float time = 0;

  App(int w, int h, int argc, char ** argv) : Renderer(w,h) {
      CONTEXT::System -> Initialize(argc, argv);
      CONTEXT::template Create(this,w,h);
      Renderer::initGL(Renderer::GL, Renderer::BUFFERED);
      init();
      process = new gfx::MotionTrace(w,h,this);  
  }

  void start(){
    CONTEXT::System -> Start();
  }

  void init(){
    mbo = new MBO( Mesh::Circle(1,10) );  
    scene.camera.pos(0,0,5);
  }
  
  void update(){
    mbo -> mesh.moveTo( sin(time) * scene.camera.lens.width()/2.0,0,0);
    mbo -> update();
  }

  void onResize(int w, int h){
    scene.resize(w, h);
    if(process) delete process;
    process = new gfx::MotionTrace(w,h,this);
  }

  void onDraw(){
    time+=.005;    
    update();
    pipe.line(*mbo);
  }

  virtual void onFrame(){
    clear();
    (*process)();
  }
};

int main(int argc, char ** argv){
  App<Window> app(500,500, argc, argv);
  app.start();
  return 0;
}
