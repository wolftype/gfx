/*
 * =====================================================================================
 *
 *       Filename:  gfx_app.h
 *
 *    Description:  an appclass that can take a variety of window contexts
 *
 *        Version:  1.0
 *        Created:  05/29/2014 07:18:12
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

template<class WINDOWCONTEXT>
struct App : Renderer {

  MBO * mbo;
  float time = 0;

  App(int w, int h, int argc = 0, char ** argv = NULL) : Renderer(w,h) {
      WINDOWCONTEXT::System -> Initialize(argc, argv);
      WINDOWCONTEXT::template Create(this,w,h);
      
      Renderer::initGL(Renderer::GL, Renderer::BUFFERED, 
                       WINDOWCONTEXT::window().width(), 
                       WINDOWCONTEXT::window().height() );
  }

  void start(){
    WINDOWCONTEXT::System -> Start();
  }

  virtual void onDraw() = 0;
  virtual void update(){}

  virtual void onFrame(){
    update();
    clear();
    render();
    WINDOWCONTEXT::SwapBuffers();
  }

};


