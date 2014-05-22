/*
 * =====================================================================================
 *
 *       Filename:  xGlutImmediate.h
 *
 *    Description:  glut windowing in immediate mode.  draws a triangle
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

using namespace gfx;

/*-----------------------------------------------------------------------------
 * example use of gfx_renderer in immediate mode
 * 1 - call Renderer::initGL() after initializing window context 
 *-----------------------------------------------------------------------------*/
template<class CONTEXT>
struct App : Renderer {

  App(int w, int h) : Renderer(w,h) {
      CONTEXT::template Create(this, w,h);
      Renderer::initGL( Renderer::GL, Renderer::IMMEDIATE );
      init();
  }

  void init(){
    scene.camera.pos(0,0,1);
  }

  void onDraw(){
    clear();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();                                         //<-- Load Identity Matrix
    gluPerspective(60, CONTEXT::window().ratio(), 1, 100);    //<-- Lens Angle, Aspect Ratio, Near Clip, Far Clip

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();                                         //<-- Load Identity Matrix

    gluLookAt( 0, 0, 1,                                       //<-- Eye position (positive is TOWARDS you)
               0, 0,-1,                                       //<-- Direction we are looking in (negative is into the screen)
               0, 1, 0);                                      //<-- Which way is Up?     }

    glBegin(GL_TRIANGLES);
      glColor3f(1,0,0); 
      glVertex3f(-.5,-.5,0);
      glColor3f(0,1,0);
      glVertex3f(0,.5,0);
      glColor3f(0,0,1);
      glVertex3f(.5,-.5,0); 
    glEnd();
  }
};

int main(int argc, char ** argv){
  Glut::Initialize(argc,argv);
  App<Window> app(500,500);
  Glut::Start();
  return 0;
}

