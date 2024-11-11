/**
 * Example of attaching a GLV based gui
 */

#include "util/gfx_imgui_app.h"
#include "util/glfw_window.hpp"

using namespace gfx;

struct MyApp : GFXAppImGui {

  MBO mbo;

  float amt1;
  bool bToggle;

  void onDrawGui() override {
    gui(amt1, "amt1", 0, 1);
    gui(bToggle, "bToggle");
  }

  void onSetup() override {
    mbo = Mesh::Circle();
    mRenderGraph.immediate(false);
  }

  void onDraw() override { draw(mbo, 1, 0, 0); }
};

int main() {

  MyApp app;
  app.start();
  return 0;
}
