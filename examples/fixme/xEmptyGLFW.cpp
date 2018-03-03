
#include "gfx_app.h"
#include "util/glfw_window.hpp"

using namespace gfx;

struct MyApp : GFXApp<GLFWContext> {

	MBO mbo;

	virtual void setup() {

		printf("SETUP SCENE\n");
		mbo = Mesh::Circle();

		mRenderGraph.immediate(true); ///< set immediate mode fixed function or advanced programming pipeline
	}

	virtual void onDraw() {

		draw(mbo,1,0,0);
		//render::begin(1, 1, 1, 1);
		//mesh::drawElements(mbo.mesh);
	}


};


int main() {

	MyApp app;
	app.start();
	return 0;
}