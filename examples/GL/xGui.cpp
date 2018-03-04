/**
  * Example of attaching a GLV based gui
  */

#include "util/gfx_glv_app.h"
#include "util/glut_window.hpp"

using namespace gfx;

struct MyApp : GFXAppGui <GlutContext>
{

  MBO mbo;

  float amt1;
  bool bToggle;

  virtual void setup ()
  {

    gui(amt1,"amt1",-100,100)(bToggle,"bToggle");

    printf ("MYAPP SETUP\n");
    mbo = Mesh::Circle ();

    mRenderGraph.immediate (true);


    Quat qa (PI * 1.25 / 2.0, Vec3f (0.0, 1.0, 0.0));
    Quat qb (PI * -.25 / 2.0, Vec3f (0.0, 1.0, 0.0));

    Vec3f nz = Quat::spin (Vec3f (0.0, 0.0, 1.0), qa);

    cout << qa << endl;
    cout << qb << endl;

    Quat qc (PI * -.25 / 2.0, nz);

    cout << nz << endl;
    cout << qc << endl;

    Quat nq = qc * qa;
    Vec3f nx = Quat::spin (Vec3f (1.0, 0.0, 0.0), nq);

    cout << nq << endl;


    cout << nq * Quat (0, 1, 0, 0) << endl;
    cout << nx << endl;
  }

  virtual void onDraw ()
  {

    draw (mbo, 1, 0, 0);
    //render::begin(1,1,1,1);
    //mesh::drawElements( mbo.mesh );
  }
};


int main ()
{

  MyApp app;
  app.start ();
  return 0;
}
