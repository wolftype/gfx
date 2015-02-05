/*
 * =====================================================================================
 *
 *       Filename:  xRenderToTexture.cpp
 *
 *    Description:  Render to a Texture and then Draw Texture on Screen
 *
 *        Version:  1.0
 *        Created:  01/28/2015 16:28:28
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

struct R2T : GFXRenderNode {
 RenderToTexture r2t;
 Blur blur;

 void init(){
    r2t.set(width,height);    
    r2t.init();

    blur.set(width,height);
    blur.init();
    blur.texture = r2t.texture;

    blur << r2t;
    
    //bind downstream and upstream
    bindDownstream(blur);
    bindUpstream(r2t);

    //blur.downstream(mDownstream);
    //for (auto& i : mUpstream) r2t << i;
 }

 void onRender(){
   blur.onRender();
 }

};


struct MyApp : GFXApp<GlutContext> {

 MBO mbo;

 //RenderToTexture r2t;
 //Blur blur;

 R2T r2t;

 virtual void setup(){

    mbo = Mesh::Sphere();
    mRenderer.immediate(false); 


   // blur.init();
  //  blur.texture = r2t.texture;
    /* slab.init(); */
    /* slab.texture = r2t.texture; */

   // mRenderer << r2t << this;

    mRenderer << r2t << mSceneRenderer;

    r2t.set(width,height);    
    r2t.init();

 }

 virtual void onDraw(){

     draw(mbo,1,0,0);

     static float time = 0; time+=.1;
     r2t.blur.ux = sin(time);
     r2t.blur.uy = cos(time/2);

 }


};


int main(){

  MyApp app;
  app.start();
  return 0;
}
