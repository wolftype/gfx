#include "gfx_app.h"
#include "util/glut_window.hpp"

using namespace gfx;

struct MyApp : App<Window> {


 virtual void init(){


 }

 virtual void onDraw(){

 }

 virtual bool onMouseDown( const Mouse& m){
  std::cout << "mouse down" << std::endl;
  return true; 
}  

};



int main(){

  MyApp app;
  app.start();
  return 0;
}
