
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

#include "util/glut_window.hpp"
#include "gfx_app.h"

using namespace gfx;

typedef GlutContext CONTEXT;


/*-----------------------------------------------------------------------------
 * example use of gfx_renderer with mesh buffer objects and motion trace
 *-----------------------------------------------------------------------------*/
struct MyApp : GFXApp<CONTEXT> {

  MBO mbo;
  float time = 0;

   MotionBlur motionblur;


  void setup(){
    glLineWidth(10);
    mbo = MBO( Mesh::Sphere() );

    mRenderer.immediate(false);  

    mRenderer << motionblur << mSceneRenderer;

    //init after
    motionblur.set( width, height);
    motionblur.init();

  }
  
   void onAnimate(){
    time +=.015;
    mbo.mesh.moveTo( sin(time) * 3,0,0);
    mbo.update();
  }
  
   void onDraw(){
    draw(mbo);
  }

};


int main(int argc, char ** argv){
  MyApp app;//1600,1200, "test");
  app.start();
  return 0;
}
