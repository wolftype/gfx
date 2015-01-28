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

#ifndef  GlutWindow_INC
#define  GlutWindow_INC

#include "gfx_lib.h"
#include "gfx_control.h"

#include <iostream>
#include <stdio.h>
#include <vector>

using namespace std;


namespace gfx{


template<class CONTEXT> 
struct GlutInterface : Interface<CONTEXT> {
  
  static void Draw(void){
    Interface<CONTEXT>::OnDraw();
  }
  static void OnReshape(int w, int h){
    Interface<CONTEXT>::OnResize(w,h);
  }

  
  static void OnMouse(int button, int state, int x, int y){
    Mouse mouse(button,state,x,y);
    Interface<CONTEXT>::io.keyboard.modifier = glutGetModifiers();
    if (state == GLUT_DOWN) {
      mouse.state |= Mouse::IsDown;
      Interface<CONTEXT>::OnMouseDown(mouse);
    }
    if (state == GLUT_UP) {
      mouse.state = 0;//Mouse::IsDown;
      Interface<CONTEXT>::OnMouseUp(mouse);
    }
  }

  static void OnMotion(int x, int y){
    Mouse mouse (0, Mouse::IsDown | Mouse::IsMoving, x, y);
    Interface<CONTEXT>::OnMouseDrag(mouse);    
  }

  static void OnPassiveMotion(int x, int y){
    Mouse mouse(0, Mouse::IsMoving, x, y);
    Interface<CONTEXT>::OnMouseMove(mouse);
  }

  static void OnKeyboardDown(unsigned char key, int x, int y){
    Keyboard keyboard(key,glutGetModifiers(),x,y,true);
    Interface<CONTEXT>::OnKeyDown(keyboard); 
 }

  static void OnKeyboardUp(unsigned char key, int x, int y){
    Keyboard keyboard(key,glutGetModifiers(),x,y,false);
    Interface<CONTEXT>::OnKeyUp(keyboard);
  }
    
  static void OnSpecialDown(int key, int x, int y){
    Keyboard keyboard(key,glutGetModifiers(),x,y,true);
    Interface<CONTEXT>::OnKeyDown(keyboard);   
  }

  static void OnSpecialUp(int key, int x, int y){
    Keyboard keyboard(key,glutGetModifiers(),x,y,false);
    Interface<CONTEXT>::OnKeyUp(keyboard);  
  }     

};

  
/*!
 *  Singleton initializer
 */
struct Glut {

  static Glut& Initialize(){
    static Glut TheGlutInstance;
    return TheGlutInstance; 
  }

  template<class APPLICATION>
  static void Start(APPLICATION*){
    glutMainLoop();
  }

  static void Terminate(){}

  private:
    Glut(){
      int argc = 1;
      char c[] = {'G','F','X'};
      char * argv[] = {c,NULL};
      glutInit(&argc,argv);
      glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    }

};


/*!
 *  A GlutContext can have a bunch of windows, 
 *
 */
struct GlutContext { 
  
  static Glut * System;

  GlutInterface<GlutContext> interface;
  static vector<WindowData*> mWindows;
  static int currentWindow;

  void create(int w, int h, string name = "glut window", int offsetW = 0, int offsetH = 0){
    
    int id = glutCreateWindow(name.c_str());
           
    //WINDOW CALLBACKS
    glutDisplayFunc(GlutInterface<GlutContext>::Draw);         
    glutTimerFunc(10, Animate, 1);
    glutReshapeFunc(Reshape);
    glutReshapeWindow(w,h);
    glutPositionWindow(offsetW, offsetH);


    //MOUSE INPUT CALLBACKS
    glutMouseFunc( GlutInterface<GlutContext>::OnMouse);
    glutMotionFunc( GlutInterface<GlutContext>::OnMotion);
    glutPassiveMotionFunc( GlutInterface<GlutContext>::OnPassiveMotion);

    //KEYBOARD INPUT CALLBACKS
    glutKeyboardFunc( GlutInterface<GlutContext>::OnKeyboardDown );
    glutKeyboardUpFunc(GlutInterface<GlutContext>::OnKeyboardUp );
    glutSpecialFunc( GlutInterface<GlutContext>::OnSpecialDown );
    glutSpecialUpFunc( GlutInterface<GlutContext>::OnSpecialUp );

    mWindows.push_back( new WindowData(w,h,id) );
    currentWindow = id-1;
  }

  static float ratio(){ 
    return mWindows[currentWindow] -> ratio(); 
  }

  static WindowData& windowData(){
    return *mWindows[currentWindow];
  }

  static void ToggleFullScreen(){
    if (!windowData().isFullScreen) {
      windowData().save();
      glutFullScreen();
    }
    else glutReshapeWindow( windowData().prevWidth(), windowData().prevHeight() );
    windowData().isFullScreen = !windowData().isFullScreen;
  }

  static void Animate(int){
    glutPostRedisplay();
    //glutSwapBuffers(); ///< do not call swap buffers here -- let interface Draw func call context's swapbuffers...
    glutTimerFunc(20, Animate, 0);
  }

  static void SwapBuffers(){
    glutSwapBuffers();
  }

  static void Reshape(int width, int height){
    currentWindow = glutGetWindow()-1;
    reshape(width,height);
    GlutInterface<GlutContext>::OnReshape(width,height);
  }

  static void reshape(int w, int h){ 
    mWindows[currentWindow] -> reshape(w,h); 
    glViewport(0,0,w,h);
  }

  void setViewport(){
    glViewport(0,0, windowData().width(),windowData().height());  
   } 
};

Glut * GlutContext::System;
vector<WindowData*> GlutContext::mWindows;
int GlutContext::currentWindow;


} //gfx::

#endif   /* ----- #ifndef GlutWindow_INC  ----- */
