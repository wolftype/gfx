/*
 * =====================================================================================
 *
 *       Filename:  lynda_window.h
 *
 *    Description:  a window wrapper around glfwWindow
 *
 *        Version:  1.0
 *        Created:  02/07/2014 11:45:18
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto 
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef GLFW_WINDOW_H_INCLUDED
#define GLFW_WINDOW_H_INCLUDED

#include <stdlib.h>
#define GLEW_STATIC
#include <GL/glew.h>
#include "GLFW/glfw3.h"

#include <iostream>


#include "gfx_control.h"

using namespace std;
namespace gfx{


/*-----------------------------------------------------------------------------
 *  Some Callbacks to be implemented later 
 *-----------------------------------------------------------------------------*/

struct GLFWContext;

struct GLFWInterface : Interface<GLFWContext> {

  static void OnKeyDown(GLFWwindow* window, int key, int scancode, int action, int mods){
      //((APPLICATION*)(app))->onKeyDown(key,action);
      Keyboard keyboard(key, mods, 0, 0, true);
      Interface<GLFWContext>::OnKeyDown(keyboard);   
  }

  static void OnMouseMove(GLFWwindow* window, double x, double y){
      Mouse mouse (0, Mouse::IsMoving, x, y);
      Interface<GLFWContext>::OnMouseMove(mouse);
      //((APPLICATION*)(app))->onMouseMove(x,y);  
  }

  static void OnMouseDown(GLFWwindow* window, int button, int action, int mods){
      Mouse mouse;
      mouse.state |= Mouse::IsDown;
      Interface<GLFWContext>::OnMouseDown(mouse);

     // ((APPLICATION*)(app))->onMouseDown(button,action);
  }

};

/*!
 *  Singleton initializer
 */
struct GLFW {

  static GLFW& Initialize(){////int argc, char ** argv){
    static GLFW TheGLFW;
    return TheGLFW;
  }

  template<class APPLICATION>
  static void Start(APPLICATION * app){
    printf("starting ...\n");
    while( !app->context().shouldClose() ){//!win.shouldClose() ){
      app->onFrame();
      app->context().pollEvents();
    }
  }

  static void Terminate(){
    glfwTerminate();
  }

  private:
    GLFW() {
      if( !glfwInit() ) exit(EXIT_FAILURE); 
    }
};

/*-----------------------------------------------------------------------------
 *  A GLFW Window Wrapper
 *-----------------------------------------------------------------------------*/
struct GLFWContext {

    static GLFW * System;

    GLFWwindow * mWindow;
    
    GLFWInterface interface;
    
    static vector<WindowData*> mWindows;
    static int currentWindow;
    WindowData& windowData(){ if (!mWindows.empty()) return *mWindows[0]; }

    int mWidth, mHeight;

    int width()  { return mWidth; }
    int height() { return mHeight; }
    float ratio() { return (float)mWidth/mHeight;}

    GLFWwindow& window() { return *mWindow; }

    GLFWContext() {}

    //Create a Window Context
    void create(int w, int h, const char * name="demo"){
        
        mWidth = w; mHeight = h;

        mWindow = glfwCreateWindow(w,h,name,NULL ,NULL);

        if (!mWindow) {
          glfwTerminate();
          exit(EXIT_FAILURE);
        }        
        
        glfwMakeContextCurrent(mWindow);
        glfwSwapInterval(1); //<-- force interval (not guaranteed to work with all graphics drivers)

        //register callbacks for keyboard and mouse
        /* glfwSetKeyCallback(mWindow, GLFWInterface::OnKeyDown); */
        /* glfwSetCursorPosCallback(mWindow, GLFWInterface::OnMouseMove ); */
        /* glfwSetMouseButtonCallback(mWindow, GLFWInterface::OnMouseDown ); */

        mWindows.push_back( new WindowData(w,h,0) );
        //currentWindow = id-1;

    }


    //Get the Current framebuffer Size in pixels and Set the Viewport to it    
    void setViewport(){ 
        glfwGetFramebufferSize(mWindow, &mWidth, &mHeight); 
        glViewport(0,0,mWidth,mHeight);  
    }

    //Check whether window should close
    bool shouldClose(){
      return glfwWindowShouldClose(mWindow);
    }

    //Swap front and back buffers
    static void SwapBuffers(){
      //glfwSwapBuffers(mWindow);
    }

    //listen
    void pollEvents(){
      glfwPollEvents();
    }

    //Destroy the window
    void destroy(){
      glfwDestroyWindow(mWindow);
    }

    ~GLFWContext(){
      destroy();
    }
};

GLFW * GLFWContext::System;
//GLFWwindow * GLFWContext::mWindow;
vector<WindowData*> GLFWContext::mWindows;
int GLFWContext::currentWindow;


} //gfx


#endif
