/*
 * =====================================================================================
 *
 *       Filename:  xCubeMap.cpp
 *
 *    Description:  render a scene to a cube map and draw spherical projection onto screen
 *
 *        Version:  1.0
 *        Created:  02/02/2015 18:53:57
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization:  pretty awesome 
 *
 * =====================================================================================
 */


#include "gfx_app.h"
#include "gfx_effects.h"

#include "util/glut_window.hpp"
#include "util/gfx_stat.h"

using namespace gfx;

struct MyApp : GFXApp<GlutContext> {

 MyApp() : GFXApp<GlutContext>(800,400,"name",false) {}

 MBO mbo;
 RenderToCubeMap cubeMapBuffer;
 CubeSlab cubeSlab;
 
 GFXStereoNode stereo;
 GFXSplitViewNode splitView;

  /// A Bunch of Triangles
  void makeMesh(){
    Rand::Seed();

    float span = 5.0;
    int num = 10;
    Mesh m;
    m.mode(GL::T);
    for (int i=0;i<num;++i){
    for (int j=0;j<num;++j){
    for (int k=0;k<num;++k){
      
      Vec3f pos = Vec3f(-span/2.0,-span/2.0,-span/2.0) + Vec3f((float)i/num,(float)j/num,(float)k/num) * span;
      Vec3f a = pos + Vec3f(0,.2,0);
      Vec3f b = pos + Vec3f(.2,.1,0);
      m.add(pos).add().add(a).add().add(b);

    }}}

    /// Randomly colored
    for (auto& i : m.vertex() ){
      i.Col.set( Rand::Num(), Rand::Num(), Rand::Num(), 1.0 );
    }
    m.store();
    mbo = m;

  }

  void setup(){

    makeMesh();
        
    mRenderer.reset();
    mRenderer << stereo << cubeSlab << cubeMapBuffer << mSceneNode << this;
   
    mRenderGraph.init(&mRenderer, 800,400,0,GFXRenderGraph::ANAGLYPH );//IMMEDIATE);

    cubeSlab.texture = cubeMapBuffer.texture; //bind textures


  }


  void onDraw(){    
    draw(mbo,1,0,0);
  }

};



int main(){
  MyApp app;
  app.start();
  return 0;
}
