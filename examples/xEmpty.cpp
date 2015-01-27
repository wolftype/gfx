

//#include "util/glfw_window.hpp"
#include "gfx_app.h"
#include "util/glut_window.hpp"


using namespace gfx;

struct MyApp : GFXApp<GlutContext> {

 MBO mbo;

 virtual void setup(){

    printf("SETUP SCENE\n");
    mbo = Mesh::Sphere();

    immediate(false); ///< set immediate mode fixed function or advanced programming pipeline
 }

 virtual void onDraw(){
    
    draw(mbo,1,0,0);

    static float time = 0;
    time += .01;
    float rad = 5.0;
    light = Vec3f(sin(time)*rad, cos(time)*rad, 1);
 }

 
 virtual void onMouseDown( const Mouse& m){
    std::cout << "mouse down" << std::endl;

 }  

 virtual void onKeyDown( const Keyboard& k){
    std::cout << "ket down: " << k.code << std::endl;
 }  

};


int main(){

  MyApp app;
  app.start();
  return 0;
}
