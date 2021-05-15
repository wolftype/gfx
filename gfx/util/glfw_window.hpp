/*
 * =====================================================================================
 *
 *       Filename:  glfw_window.h
 *
 *    Description:  a context wrapper around GLFW windows
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
namespace gfx {


/*-----------------------------------------------------------------------------
 *  Some Callbacks to be implemented later
 *-----------------------------------------------------------------------------*/

struct GLFWContext;

struct GLFWInterface : Interface<GLFWContext>
{
  static Mouse mouse;

  static void OnKeyDown (GLFWwindow *window, int key, int scancode, int action,
                         int mods)
  {
    Keyboard keyboard (key, mods, 0, 0, true);
    Interface<GLFWContext>::OnKeyDown (keyboard);
  }

  static void OnMouseMove (GLFWwindow *window, double x, double y)
  {
    mouse.state |= Mouse::IsMoving;
    mouse.x = x;
    mouse.y = y;
    if (mouse.state & Mouse::IsDown)
       Interface<GLFWContext>::OnMouseDrag (mouse);
    else
       Interface<GLFWContext>::OnMouseMove (mouse);
  }

  static void OnMouseDown (GLFWwindow *window, int button, int action, int mods)
  {
    if (action==GLFW_PRESS)
    {
      mouse.state |= Mouse::IsDown;
      Interface<GLFWContext>::OnMouseDown (mouse);
    } else
    {
      mouse.notState (Mouse::IsDown);
      Interface<GLFWContext>::OnMouseUp (mouse);
    }

  }

  //window
  static void OnResizeWindow(int w, int h)
  {
    io.viewdata.win_w = w;
    io.viewdata.win_h = h;
  }

  //framebuffer
  static void OnResize (int w, int h)
  {
    io.viewdata.w = w;
    io.viewdata.h = h;
    for (auto &i : mWindowEventHandlers)
      {
        i->onResize (w, h);
      }
  }
};

Mouse GLFWInterface::mouse;

/*!
 *  Singleton initializer
 */
struct GLFW
{

  static GLFW &Initialize (int mode = 0)
  {  ////int argc, char ** argv){
    static GLFW TheGLFW (mode);
    return TheGLFW;
  }

  /// Start Graphics Thread, passing application in
  /// fetches the GLFWInterface member of Application's context
  template <class APPLICATION>
  static void Start (APPLICATION *app)
  {
    printf ("starting ...\n");
    while (!app->context ().shouldClose ())
      {  //!win.shouldClose() ){
        //app->onFrame();
        app->context ().interface.OnDraw ();
        app->context ().pollEvents ();  //why not swap buffers here?
      }
  }

  static void Terminate () { glfwTerminate (); }

 private:
  GLFW (int mode)
  {
    if (!glfwInit ())
      exit (EXIT_FAILURE);
  }
};

/*!

    A GLFW Context
    Has a ::System

 */

struct GLFWContext
{

  static GLFW *System;

  static GLFWwindow *mWindow;  //one window?  many . ..
  //static vector<GLFWwindow*> mWindow; or map<GLFWwindow*, int> mWindow;
  GLFWInterface interface;

  static vector<WindowData *> mWindows;
  static int currentWindow;

  /// Get Window Information, if one exists
  /// @todo handle case of no windows created
  WindowData &windowData ()
  {
    if (!mWindows.empty ())
      return *mWindows[0];
    else
      return create (200, 200);
  }

  int mWidth, mHeight;

  int width () { return mWidth; }
  int height () { return mHeight; }
  float ratio () { return (float) mWidth / mHeight; }

  GLFWwindow &window () { return *mWindow; }

  GLFWContext () {}

  //Create a Window Context
  WindowData &create (int w, int h, string name = "default")
  {

    cout << "creating GLFW window" << endl;

    mWidth = w;
    mHeight = h;

    mWindow = glfwCreateWindow (w, h, name.c_str (), NULL, NULL);

    if (!mWindow)
      {
        glfwTerminate ();
        exit (EXIT_FAILURE);
      }

    glfwMakeContextCurrent (mWindow);
    glfwSwapInterval (
      1);  //<-- force interval (not guaranteed to work with all graphics drivers)

    //register callback when window is resized
    glfwSetWindowSizeCallback (mWindow, Reshape);

    //register callbacks for keyboard and mouse
    glfwSetKeyCallback(mWindow, GLFWInterface::OnKeyDown);
    glfwSetCursorPosCallback(mWindow, GLFWInterface::OnMouseMove );
    glfwSetMouseButtonCallback(mWindow, GLFWInterface::OnMouseDown );

    int tw, th; 
    glfwGetFramebufferSize (mWindow, &tw, &th);
    interface.io.viewdata.win_w = w;
    interface.io.viewdata.win_h = h;

    interface.io.viewdata.w = tw;
    interface.io.viewdata.h = th;

    mWindows.push_back (new WindowData (w, h, 0));
    return *mWindows.back ();
  }

  static void Reshape (GLFWwindow *win, int w, int h)
  {
    int tw, th; 
    glfwGetFramebufferSize (mWindow, &tw, &th);
    GLFWInterface::OnResize (tw, th);
    GLFWInterface::OnResizeWindow (w, h);
  }

  //just redo it again
  void reshape()
  {
    int tw, th; 
    glfwGetFramebufferSize (mWindow, &tw, &th);
    GLFWInterface::OnResize (tw, th);
  }


  //Get the Current framebuffer Size in pixels and Set the Viewport to it
  void setViewport ()
  {
    glfwGetFramebufferSize (mWindow, &mWidth, &mHeight);
    glViewport (0, 0, mWidth, mHeight);
  }

  //Check whether window should close
  bool shouldClose () { return glfwWindowShouldClose (mWindow); }

  //Swap front and back buffers
  static void SwapBuffers () { glfwSwapBuffers (mWindow); }

  //listen
  void pollEvents () {
    glfwPollEvents ();
  }

  //Destroy the window
  void destroy () { glfwDestroyWindow (mWindow); }

  ~GLFWContext () { destroy (); }
};

GLFW *GLFWContext::System;
GLFWwindow *GLFWContext::mWindow;
vector<WindowData *> GLFWContext::mWindows;
int GLFWContext::currentWindow;


}  //gfx


#endif
