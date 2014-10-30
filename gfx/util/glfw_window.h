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


using namespace std;
namespace gfx{


/*-----------------------------------------------------------------------------
 *  Some Callbacks to be implemented later 
 *-----------------------------------------------------------------------------*/
struct Interface {

  static void * app; // <-- an unknown application to be defined later

  template<class APPLICATION>
  static void OnKeyDown(GLFWwindow* window, int key, int scancode, int action, int mods){
      ((APPLICATION*)(app))->onKeyDown(key,action);
  }

  template<class APPLICATION>
  static void OnMouseMove(GLFWwindow* window, double x, double y){
      ((APPLICATION*)(app))->onMouseMove(x,y);  
  }

  template<class APPLICATION>
  static void OnMouseDown(GLFWwindow* window, int button, int action, int mods){
      ((APPLICATION*)(app))->onMouseDown(button,action);
  }

};

void * Interface::app;


struct GLFW {

  static GLFW& Initialize(){////int argc, char ** argv){
    static GLFW TheGLFW;
    return TheGLFW;
  }

  template<class APPLICATION>
  static void Start(APPLICATION * app){
    printf("starting ...\n");
    while( !app->window().shouldClose() ){
      app->onFrame();
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
struct Window {

    static GLFW * System;

    GLFWwindow * mWindow;
    Interface interface;

    int mWidth, mHeight;

    int width()  { return mWidth; }
    int height() { return mHeight; }
    float ratio() { return (float)mWidth/mHeight;}

    GLFWwindow& window() { return *mWindow; }

    Window() {}

    //Create a Window Context
    template<class APPLICATION>
    void create(APPLICATION * app, int w, int h, const char * name="demo"){
        
        interface.app = app;

        mWidth = w; mHeight = h;

        mWindow = glfwCreateWindow(w,h,name,NULL ,NULL);
        if (!mWindow) {
          glfwTerminate();
          exit(EXIT_FAILURE);
        }        
        glfwMakeContextCurrent(mWindow);
        glfwSwapInterval(1); //<-- force interval (not guaranteed to work with all graphics drivers)

        //register callbacks for keyboard and mouse
        glfwSetKeyCallback(mWindow, Interface::OnKeyDown<APPLICATION>);
        glfwSetCursorPosCallback(mWindow, Interface::OnMouseMove<APPLICATION> );
        glfwSetMouseButtonCallback(mWindow, Interface::OnMouseDown<APPLICATION> );

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
    void swapBuffers(){
      glfwSwapBuffers(mWindow);
    }

    //listen
    void pollEvents(){
      glfwPollEvents();
    }

    void onFrame(){
      swapBuffers();
      pollEvents();
    }

    //Destroy the window
    void destroy(){
      glfwDestroyWindow(mWindow);
    }


    ~Window(){
      destroy();
    }
};

GLFW * Window::System;

} //gfx


#endif
