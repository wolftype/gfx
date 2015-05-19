
#include "gfx_app.h"
#include "util/glut_window.hpp"

using namespace gfx;

struct MyApp : GFXApp<GlutContext> {

 MBO mbo;

 virtual void setup(){

    printf("SETUP SCENE\n");
    mbo = Mesh::Sphere();

    mSceneRenderer.immediate(false); ///< set immediate mode fixed function or advanced programming pipeline
 }

 virtual void onDraw(){
    
    draw(mbo,1,0,0);

 }


};


int main(){

  MyApp app;
  app.start();
  return 0;
}
