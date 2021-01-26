/*
 * =====================================================================================
 *
 *       Filename:  glut_window.h
 *
 *    Description:  A Glut Context Windowing Class
 *
 *        Version:  1.0
 *        Created:  06/09/2014 17:50:12
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *
 * =====================================================================================
 */

#ifndef GlutWindow_INC
#define GlutWindow_INC

#include "gfx_lib.h"
#include "gfx_control.h"

#include <iostream>
#include <stdio.h>
#include <vector>

using namespace std;


namespace gfx {

struct GlutContext;

struct GlutInterface : Interface<GlutContext>
{

  static void Draw (void) { Interface<GlutContext>::OnDraw (); }
  static void OnReshape (int w, int h)
  {
    Interface<GlutContext>::OnResize (w, h);
  }


  static void OnMouse (int button, int state, int x, int y)
  {
    Mouse mouse (button, state, x, y);
    Interface<GlutContext>::io.keyboard.modifier = glutGetModifiers ();
    if (state == GLUT_DOWN)
      {
        mouse.state |= Mouse::IsDown;
        Interface<GlutContext>::OnMouseDown (mouse);
      }
    if (state == GLUT_UP)
      {
        mouse.state = 0;  //Mouse::IsDown;
        Interface<GlutContext>::OnMouseUp (mouse);
      }
  }

  static void OnMotion (int x, int y)
  {
    Mouse mouse (0, Mouse::IsDown | Mouse::IsMoving, x, y);
    Interface<GlutContext>::OnMouseDrag (mouse);
  }

  static void OnPassiveMotion (int x, int y)
  {
    Mouse mouse (0, Mouse::IsMoving, x, y);
    Interface<GlutContext>::OnMouseMove (mouse);
  }

  static void OnKeyboardDown (unsigned char key, int x, int y)
  {
    Keyboard keyboard (key, glutGetModifiers (), x, y, true);
    Interface<GlutContext>::OnKeyDown (keyboard);
  }

  static void OnKeyboardUp (unsigned char key, int x, int y)
  {
    Keyboard keyboard (key, glutGetModifiers (), x, y, false);
    Interface<GlutContext>::OnKeyUp (keyboard);
  }

  static void OnSpecialDown (int key, int x, int y)
  {
    Keyboard keyboard (key, glutGetModifiers (), x, y, true);
    Interface<GlutContext>::OnKeyDown (keyboard);
  }

  static void OnSpecialUp (int key, int x, int y)
  {
    Keyboard keyboard (key, glutGetModifiers (), x, y, false);
    Interface<GlutContext>::OnKeyUp (keyboard);
  }
};


/*!
 *  Singleton initializer
 */
struct Glut
{

  static Glut &Initialize (bool bStereo)
  {
    static Glut TheGlutInstance (bStereo);
    return TheGlutInstance;
  }

  template <class APPLICATION>
  static void Start (APPLICATION *)
  {
    glutMainLoop ();
  }

  static void Terminate () {}

 private:
  Glut (bool bStereo)
  {
    int argc = 1;
    char c[] = {'G', 'F', 'X'};
    char *argv[] = {c, NULL};

    glutInit (&argc, argv);
    glutInitDisplayMode (GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH
                         | (bStereo ? GLUT_STEREO : 0));
  }
};


/*!
 *  A Context can own a bunch of windows,
 *
 */
struct GlutContext
{

  static Glut *System;

  GlutInterface interface;
  static vector<WindowData *> mWindows;
  static int currentWindow;

  void create (int w, int h, string name = "glut window", int offsetW = 0,
               int offsetH = 0)
  {

    int id = glutCreateWindow (name.c_str ());

    //WINDOW CALLBACKS
    glutDisplayFunc (GlutInterface::Draw);
    glutTimerFunc (10, Animate, 1);
    glutReshapeFunc (Reshape);
    glutReshapeWindow (w, h);
    glutPositionWindow (offsetW, offsetH);

    //MOUSE INPUT CALLBACKS
    glutMouseFunc (GlutInterface::OnMouse);
    glutMotionFunc (GlutInterface::OnMotion);
    glutPassiveMotionFunc (GlutInterface::OnPassiveMotion);

    //KEYBOARD INPUT CALLBACKS
    glutKeyboardFunc (GlutInterface::OnKeyboardDown);
    glutKeyboardUpFunc (GlutInterface::OnKeyboardUp);
    glutSpecialFunc (GlutInterface::OnSpecialDown);
    glutSpecialUpFunc (GlutInterface::OnSpecialUp);

    mWindows.push_back (new WindowData (w, h, id));
    currentWindow = id - 1;
  }

  static float ratio () { return mWindows[currentWindow]->ratio (); }

  static WindowData &windowData () { return *mWindows[currentWindow]; }

  static void ToggleFullScreen ()
  {
    if (!windowData ().isFullScreen)
      {
        windowData ().save ();
        glutFullScreen ();
      }
    else
      glutReshapeWindow (windowData ().prevWidth (),
                         windowData ().prevHeight ());
    windowData ().isFullScreen = !windowData ().isFullScreen;
  }

  static void Animate (int)
  {
    glutPostRedisplay ();
    glutTimerFunc (20, Animate, 0);
  }

  static void SwapBuffers () { glutSwapBuffers (); }

  static void Reshape (int width, int height)
  {
    currentWindow = glutGetWindow () - 1;
    reshape (width, height);
    GlutInterface::OnReshape (width, height);
  }

  void reshape()
  {
    reshape (windowData().width(), windowData().height());
  }

  static void reshape (int w, int h)
  {
    windowData().reshape (w, h);
    glViewport (0, 0, w, h);
  }

  void setViewport ()
  {
    glViewport (0, 0, windowData ().width (), windowData ().height ());
  }
};

Glut *GlutContext::System;
vector<WindowData *> GlutContext::mWindows;
int GlutContext::currentWindow;


}  //gfx::

#endif /* ----- #ifndef GlutWindow_INC  ----- */
