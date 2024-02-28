
/**
  * Example of attaching an imgui based gui
  */

#include "util/gfx_imgui_app.h"

using namespace gfx;

struct MyApp : GFXAppImGui
{

  MBO mbo;

  float amt1;
  bool bToggle;

  virtual void setup ()
  {
    GFXAppImGui::setup();

    mbo = Mesh::Circle ();
    mRenderGraph.immediate (true);

  }

  virtual void onDraw ()
  {
    GFXAppImGui::onDraw();

    draw (mbo, 1, 0, 0);
  }

};


int main ()
{

  MyApp app;
  app.start ();
  return 0;
}
