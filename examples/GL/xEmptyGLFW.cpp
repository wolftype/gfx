
#include "gfx_app.h"
#include "util/glfw_window.hpp"

using namespace gfx;

struct MyApp : GFXApp<GLFWContext> {

  MBO mbo;

  void onSetup() override {

    mbo = Mesh::Circle();

    mRenderGraph.immediate(true);
  }

  void onDraw() override { draw(mbo, 1, 0, 0); }

  void onKeyDown(const Keyboard &k) override {
    cout << "KEY DOWN " << k.code << endl;
  }
};

int main() {

  MyApp app;
  app.start();
  return 0;
}
