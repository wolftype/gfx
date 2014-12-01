/*
 * =====================================================================================
 *
 *       Filename:  xGLV.cpp
 *
 *    Description:  attach glv widgets
 *
 *        Version:  1.0
 *        Created:  10/31/2014 16:52:58
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization:  
 *
 * =====================================================================================
 */

#include "gfx_app.h"
#include "gfx_mesh.h"
#include "gfx_sceneController.h"

#include "util/glut_window.hpp"
#include "util/glv_control.h"
#include "util/glv_gui.h"

using namespace gfx;

struct MyApp : public GfxApp<GlutContext> {

  Mesh * mesh;

  //Scene scene;

  GLVBinding<GlutContext> glv;
  glv::Gui gui;

  bool bBut;
  float amtA, amtB;
  glv::Button button;

  virtual void setup(){
    mesh = new Mesh ( Mesh::Circle(1) );

    mWindow.interface.addInputEventHandler(this); 
    glv.listenTo( window() );
    glv << gui;
    gui( bBut, "hello" );
    gui( amtA, "amtA",0,100 );
    gui( amtB, "amtB",0,100 );
    bBut = 1;

  }

  virtual void onDraw(){
    
    // scene.push();
      mesh -> drawElements();
   //  scene.pop();

  }

  virtual void onMouseDown(const Mouse& m){
    cout << m.x << " " << m.y << endl;
  }

  virtual void onResize(int w, int h){
    scene.fit(w, h);
  }

};


int main(){
  MyApp app;
  app.start();
  return 0;
}
