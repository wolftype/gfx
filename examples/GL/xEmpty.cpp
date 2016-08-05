
#include "gfx_app.h"
#include "util/glut_window.hpp"

using namespace gfx;

struct MyApp : GFXApp<GlutContext> {

 MBO mbo;

 virtual void setup(){

    printf("SETUP SCENE\n");
    mbo = Mesh::Circle();

    mRenderGraph.immediate(true);
 }

 virtual void onDraw(){

    draw(mbo,1,0,0);
    //render::begin(1,1,1,1);
    //mesh::drawElements( mbo.mesh );
 }


};


int main(){

  MyApp app;
  app.start();
  return 0;
}
