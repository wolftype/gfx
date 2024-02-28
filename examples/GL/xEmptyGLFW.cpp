
#include "gfx_app.h"
#include "util/glfw_window.hpp"

using namespace gfx;

struct MyApp : GFXApp<GLFWContext> {

 MBO mbo;

 virtual void setup(){

    printf("MYAPP SETUP\n");
    mbo = Mesh::Circle();

    mRenderGraph.immediate(true);

 }

 virtual void onDraw(){
    draw(mbo,1,0,0);
 }

 void onKeyDown(const Keyboard& k){
    cout << "KEY DOWN " << k.code << endl;
 }

};


int main(){

  MyApp app;
  app.start();
  return 0;
}
