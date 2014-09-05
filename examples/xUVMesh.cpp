/*
 * =====================================================================================
 *
 *       Filename:  xUVMesh.cpp
 *
 *    Description:  uv mesh
 *
 *        Version:  1.0
 *        Created:  05/30/2014 11:35:16
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

  Mesh mesh;
  MBO * mbo;
  float time = 0;

  MyApp(int w, int h, int argc, char ** argv) : App<Window>(w,h,argc,argv) {
      init();
  }

  void init()
  
  {

    float spacing = .4;
    int w = 50; int h = 50;
    float tw = spacing * w;
    float th = spacing * h;

    Vec3f * vec = new Vec3f[w*h];

    bool bFlip =0;
    for (int i=0; i<w; ++i){
      for (int j=0; j<h; ++j){
       float u =  (float)i/w * tw + (bFlip?spacing/2.0:0);
       float v =  (float)j/h * th;
       //vec[i*h+j] = Vec3f(u,v,0);
       Vertex vert = Vec3f(-tw/2.0 +u,-th/2.0 +v,0);
       vert.Tex = Vec2f( u / (tw+spacing/2.0), v/th );
       mesh.add(vert).add();
       if(j==0) mesh.add();
       bFlip = !bFlip;
      }  
      mesh.add();
    }

    mesh.mode( GL::LS );
    mesh.store();

    
    mbo = new MBO( mesh );//Mesh::HexaGrid(w,h,.4) );  
    scene.camera.pos(0,0,5);
  }
  
  virtual void update(){
  }

  virtual void onDraw(){

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
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
