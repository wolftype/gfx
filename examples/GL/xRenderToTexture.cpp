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

struct R2T : GFXRenderNode
{

  RenderToTexture r2t;
  Blur blur;

  void onInit ()
  {

    r2t.init (width, height, mRenderGraph);
    blur.init (width, height, mRenderGraph);

    //set texture
    blur.texture = r2t.texture;

    blur << r2t;

    //bind downstream
    divert (blur);  // blur points to this instance's downstream process
    //bind upstream
    channel (r2t);  // r2t calls this instance's upstream processes
  }

  void onRender () { blur.onRender (); }
};


struct MyApp : GFXApp<GlutContext>
{

  MBO mbo;
  R2T r2t;

  virtual void setup ()
  {

    mbo = Mesh::Sphere ();

    mRenderer.clear ();
    mRenderer << r2t << mShaderNode << mSceneNode << this;

    mRenderGraph.init (&mRenderer, 800, 400);
  }

  virtual void onDraw ()
  {

    draw (mbo, 1, 0, 0);

    static float time = 0;
    time += .01;
    r2t.blur.ux = sin (time);
    r2t.blur.uy = cos (time / 2);
  }
};


int main ()
{

  MyApp app;
  app.start ();
  return 0;
}
