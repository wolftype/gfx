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
#include "gfx_renderer.h"

using namespace gfx;
/*-----------------------------------------------------------------------------
 *  A test to pass to Window class (should draw a triangle)
 *-----------------------------------------------------------------------------*/
struct Test : Renderer {

  static void onDraw(int w, int h){
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();                      //<-- Load Identity Matrix
    gluPerspective(60, (float)w/h, 1, 100);   //<-- Lens Angle, Aspect Ratio, Near Clip, Far Clip

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();                     //<-- Load Identity Matrix
    gluLookAt( 0, 0, 1,                   //<-- Eye position (positive is TOWARDS you)
               0, 0,-1,                   //<-- Direction we are looking in (negative is into the screen)
               0, 1, 0);                  //<-- Which way is Up?   

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
  Window<Test>::Create(500,500);
  Glut::Start();
  return 0;
}
