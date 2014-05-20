/*
 * =====================================================================================
 *
 *       Filename:  xImmediateMode.cpp
 *
 *    Description:  Render in immediate mode
 *
 *        Version:  1.0
 *        Created:  05/19/2014 12:27:42
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization:  
 *
 * =====================================================================================
 */

#include "gfx_renderer.h"
#include "util/GLFWWindow.h"

//GraphicsApp inherits from GLFW Window and GFX Renderer
struct GraphicsApp : public Window, public Renderer {

    float time;

    GraphicsApp( int w, int h, int z = 30.0 ) : Window(640,480), Renderer(false, w,h,z),
    time(0.0)
    {
      contextWidth = mWidth;
      contextHeight = mHeight;

      init();

      //cout << mWidth << " " << contextHeight << endl; 
    }

    virtual void onFrame(){
        
      time += .05;                      //<-- INCREMENT COUNTER

      Renderer::onFrame();
      
      swapBuffers();          //<-- window swap buffers
      pollEvents();           //<-- window handle events  

    }

    void start(){
      while(!shouldClose()){
        onFrame();
      }
    }

    void stop(){
      glfwTerminate();
    }

    ~App(){ stop(); }
};

int main(){
  
  printf("hello app\n");

  // Initialize GLFW
  if (!glfwInit()) return(0); 
  
  // Construct App and start loop
  App app(20,10);
  app.start();

  printf("goodbye app\n");        
  
  return 0;
}


