/*
 * =====================================================================================
 *
 *       Filename:  xSlab.cpp
 *
 *    Description:  generate a random texture and draw to screen
 *
 *        Version:  1.0
 *        Created:  01/28/2015 12:25:53
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization:  pretty awesome
 *
 * =====================================================================================
 */

#include "gfx_app.h"
#include "gfx_effects.h"
#include "util/glut_window.hpp"

using namespace gfx;

struct MyApp : GFXApp<GlutContext>
{

  MBO mbo;
  Slab slab;

  virtual void setup ()
  {

    mbo = Mesh::Sphere ();

    slab.init (10, 10, &mRenderGraph);

    float *cval = new float[slab.width * slab.height * 4];

    for (int i = 0; i < slab.width; ++i)
      {
        for (int j = 0; j < slab.height; ++j)
          {
            cval[i * slab.height * 4 + j * 4] = (float) i / slab.width;
            cval[i * slab.height * 4 + j * 4 + 1] = (float) j / slab.height;
            cval[i * slab.height * 4 + j * 4 + 2] = (float) j / i;
            cval[i * slab.height * 4 + j * 4 + 3] = .5;
          }
      }

    slab.texture->update (cval);
  }

  virtual void onDraw ()
  {

    draw (mbo, 1, 0, 0);

    static float time = 0;
    time += .01;
    float rad = 5.0;
    light = Vec3f (sin (time) * rad, cos (time) * rad, 1);

    slab.onRender ();
  }
};


int main ()
{

  MyApp app;
  app.start ();
  return 0;
}
