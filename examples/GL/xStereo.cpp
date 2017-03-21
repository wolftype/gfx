/*
 * =====================================================================================
 *
 *       Filename:  xStereo.cpp
 *
 *    Description:  Anaglyph stereo (red / cyan)
 *
 *        Version:  1.0
 *        Created:  02/22/2016 16:50:31
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail->wolftype
 *   Organization:  wolftype
 *
 * =====================================================================================
 */

#include "gfx_app.h"
#include "util/glut_window.hpp"

 using namespace gfx;

struct MyApp : GFXApp<GlutContext> {

  MBO circle =  Mesh::Circle();

  GFXStereoNode mStereo;

  virtual void setup(){

      mRenderer.reset();

      mRenderer << mStereo << mSceneNode << this;
     
      mRenderGraph.init(&mRenderer, 800,400, GFXRenderGraph::IMMEDIATE, GFXRenderGraph::ANAGLYPH );
      mRenderGraph.immediate(true);
  }

  virtual void onDraw(){
    draw (circle, 1,1,1);
  }

};




int main(){
  MyApp app;

  app.start();

  return 0;

}
