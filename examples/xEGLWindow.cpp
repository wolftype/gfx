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

#include "gfx_renderer.h"
#include "gfx_mbo.h"
#include "gfx_process.h"

using namespace gfx;

//SINGLETON
struct BCM {
  
  static BCM& Initialize(int argc, char**argv){
    static BCM TheBCM;
    return TheBCM;
  }

  template<class APP>
  void Start(APP * app){
    while(true){       
         app -> drawFunc();
         usleep(166);
      }    
  }

   ~BCM() {
     bcm_host_deinit();
     cout << "bcm_host_deinit()\n";
   }

  private: 
    BCM() {
      bcm_host_init();
      cout << "bcm_host_init()\n";
    }
};

//Context
struct RPIContext {

   static BCM * System;
   static EGL::Window * mWindow;
  
   template<class APPLICATION>
   static void Create(APPLICATION * _app, int w, int h){
     mWindow = new EGL::Window();
   }

   static EGL::Window& window() { return *mWindow; }

   ~RPIContext(){
     delete mWindow;
     delete System;
   }
};

EGL::Window * RPIContext::mWindow;
BCM * RPIContext::System;

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
      int _w = CONTEXT::window().width();
      int _h = CONTEXT::window().height();
      Renderer::initGL(Renderer::GLES, Renderer::BUFFERED);
      init();
      process = new gfx::MotionTrace(_w,_h,this);  
  }


  void start(){
    CONTEXT::System -> Start(this);
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
  App<RPIContext> app(500,500, argc, argv);
  app.start();
  return 0;
}

