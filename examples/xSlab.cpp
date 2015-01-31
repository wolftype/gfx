/*
 * =====================================================================================
 *
 *       Filename:  xSlab.cpp
 *
 *    Description:  generate a random texture and draw to screen
 *
 *        Version:  1.0
 *        Created:  01/28/2015 12:25:53
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization:  pretty awesome 
 *
 * =====================================================================================
 */

#include "gfx_app.h"
#include "util/glut_window.hpp"

using namespace gfx;

struct MyApp : GFXApp<GlutContext> {

 MBO mbo;
 Slab slab;

 virtual void setup(){

    mbo = Mesh::Sphere();
    mRenderer.immediate(false); 

    slab.init();
    
//    float * cval = new float[slab.width*slab.height*4];
    unsigned char * cval = new  unsigned char [slab.width*slab.height*4];
        
    for (int i =0;i<slab.width;++i){
      for (int j=0;j<slab.height;++j){
          cval[i*slab.height*4+j*4] = 255*(float)i/slab.width;
          cval[i*slab.height*4+j*4+1] = 255*(float)j/slab.height;
          cval[i*slab.height*4+j*4+2] = 255*(float)j/i;
          cval[i*slab.height*4+j*4+3] = 128; 
          
      }
    }

    slab.texture->data(cval);
    slab.texture->update();
 }

 virtual void onDraw(){
    
    draw(mbo,1,0,0);

    static float time = 0;
    time += .01;
    float rad = 5.0;
    light = Vec3f(sin(time)*rad, cos(time)*rad, 1);

    slab.onRender();

 }


};


int main(){

  MyApp app;
  app.start();
  return 0;
}
