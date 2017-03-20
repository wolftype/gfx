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

 // StereoScene mStereoSceneNode;
  GFXStereoNode mStereo;
  GFXSplitViewNode mSplitView;
  GFXFrameBufferNode mBuffer;

  virtual void setup(){

      mRenderer.reset();
    
    //mStereoSceneNode.mScenePtr = &scene;
      mRenderer << mStereo << mSplitView << mSceneRenderer << mSceneNode << this;
     
      mRenderGraph.init(800,400, GFXRenderGraph::IMMEDIATE, GFXRenderGraph::SPLIT );
    
     // mRenderer.immediate(true);
  }

  virtual void onDraw(){
   // scenegraph.onRender(); 
    render::draw(circle);

  }

};




int main(){
  MyApp app;

  app.start();

  return 0;

}
