/*
 * =====================================================================================
 *
 *       Filename:  glut_window.h
 *
 *    Description:  A Windowing Class for Up and Running with OpenGL
 *
 *        Version:  1.0
 *        Created:  06/09/2014 17:50:12
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef  GlutWindow_INC
#define  GlutWindow_INC

#include "gfx_lib.h"
#include "util/gfx_event.h"

#include <iostream>
#include <stdio.h>
#include <vector>

using namespace std;


namespace gfx{

/*!
 *  Stores width, height, and id of a Window
 */
struct WindowData {
  WindowData(int width = 100, int height = 100, int id = 0) : 
    mWidth(width), mHeight(height), mID(id), isFullScreen(false){}
  int mWidth, mHeight, mPrevWidth, mPrevHeight, mID;
  bool isFullScreen;
  float ratio() { return (float) mWidth/mHeight; }
  void reshape(int w, int h){
    mWidth = w; mHeight =h;
  }
  void save(){
    mPrevWidth = mWidth;
    mPrevHeight = mHeight;
  }
  int width() const { return mWidth; }
  int height() const { return mHeight; }
  int prevWidth() const { return mPrevWidth; }
  int prevHeight() const { return mPrevHeight; }
};


/*-----------------------------------------------------------------------------
 *  Some Callbacks to be implemented later 
 *-----------------------------------------------------------------------------*/
template <class CONTEXT>
struct Interface {

  static vector<InputEventHandler<CONTEXT>*> mInputEventHandlers;
  static vector<WindowEventHandler<CONTEXT>*> mWindowEventHandlers;
  
  Interface& addInputEventHandler( InputEventHandler<CONTEXT> * e ){ 
     mInputEventHandlers.push_back(e); 
     return *this; 
   } 
  Interface& addWindowEventHandler( WindowEventHandler<CONTEXT> * e ){ 
     mWindowEventHandlers.push_back(e); 
     return *this; 
   } 

  static void OnDraw(void){
    for (auto& i : mWindowEventHandlers){
      i->onFrame();
    }
  }

  static void OnMouse(int button, int state, int x, int y){
    Mouse mouse(button,state,x,y);
    if (state == GLUT_DOWN) {
      for (auto& i : mInputEventHandlers){
          i->onMouseDown(mouse);
      }
    }
    if (state == GLUT_DOWN) {
      for (auto& i : mInputEventHandlers){
          i->onMouseUp(mouse);
      }
    }
  }

  static void OnMouseDrag(int x, int y){
    Mouse mouse (0, Mouse::IsDown & Mouse::IsMoving, x, y);
    for (auto& i : mInputEventHandlers){ i->onMouseDrag(mouse); }
  }

  static void OnMouseMove(int x, int y){
    Mouse mouse(0, Mouse::IsMoving, x, y);
    for (auto& i : mInputEventHandlers){ i->onMouseMove(mouse); }
  }

  static void OnKeyboardDown(unsigned char key, int x, int y){
    Keyboard keyboard(key,x,y);
   for (auto& i : mInputEventHandlers){ i->onKeyDown(keyboard); }   
 }

  static void OnKeyboardUp(unsigned char key, int x, int y){
     Keyboard keyboard(key,x,y);
     for (auto& i : mInputEventHandlers){ i->onKeyUp(keyboard); }   
  } 
  
  static void OnSpecialDown(int key, int x, int y){
     Keyboard keyboard(key,x,y);
     for (auto& i : mInputEventHandlers){ i->onKeyDown(keyboard); }   
  }

  static void OnSpecialUp(int key, int x, int y){
     Keyboard keyboard(key,x,y);
     for (auto& i : mInputEventHandlers){ i->onKeyUp(keyboard); }   
  }     

};

/* template<class CONTEXT> */
/* void * Interface<CONTEXT>::app; */ 

template<class CONTEXT>
vector<InputEventHandler<CONTEXT>*> Interface<CONTEXT>::mInputEventHandlers;

template<class CONTEXT>
vector<WindowEventHandler<CONTEXT>*> Interface<CONTEXT>::mWindowEventHandlers;
  
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
 *  Window can have a bunch of windows, 
 *
 *  Rename to GlutContext?
 */
struct Window { //GlutContext {
  
  static Glut * System;

  Interface<Window> interface;
  static vector<WindowData*> mWindows;
  static int currentWindow;

 // template<class APPLICATION>
 // void create(APPLICATION * _app, 
    
    void create(int w, int h, int offsetW = 0, int offsetH = 0){
    
    int id = glutCreateWindow("glut");
   // cout <<"INITIALIZING WINDOW " << id << " " <<  w << " " << h << endl;         
  //  interface.app = _app;

           
    //WINDOW CALLBACKS
    glutDisplayFunc(Interface<Window>::OnDraw);//<APPLICATION>);           //<-- Callbacks Bind to Application
    glutTimerFunc(10, Animate, 1);
    glutReshapeFunc(  Reshape);

    glutReshapeWindow(w,h);
    glutPositionWindow(offsetW, offsetH);


    //MOUSE INPUT CALLBACKS
    glutMouseFunc( Interface<Window>::OnMouse);
    glutMotionFunc( Interface<Window>::OnMouseDrag);
    glutPassiveMotionFunc( Interface<Window>::OnMouseMove);

    //KEYBOARD INPUT CALLBACKS
    glutKeyboardFunc( Interface<Window>::OnKeyboardDown );
    glutKeyboardUpFunc(Interface<Window>::OnKeyboardUp );
    glutSpecialFunc( Interface<Window>::OnSpecialDown );
    glutSpecialUpFunc( Interface<Window>::OnSpecialUp );

    mWindows.push_back( new WindowData(w,h,id) );
    currentWindow = id-1;
    //return currentWindow;
  }

  static float ratio(){ 
    return mWindows[currentWindow] -> ratio(); 
  }

  static WindowData& window(){
    return *mWindows[currentWindow];
  }

  static void ToggleFullScreen(){
    cout << "TOGGLE" << endl;
    if (!window().isFullScreen) {
      window().save();
      glutFullScreen();
    }
    else glutReshapeWindow( window().prevWidth(), window().prevHeight() );
    window().isFullScreen = !window().isFullScreen;
  }

  static void Animate(int){
    glutPostRedisplay();
    glutSwapBuffers();
    glutTimerFunc(20, Animate, 0);
  }

  static void SwapBuffers(){
    glutSwapBuffers();
  }

  static void Reshape(int width, int height){
    currentWindow = glutGetWindow()-1;
    reshape(width,height);
    //((APPLICATION*)(Interface::app) ) -> onResize(width,height);//((float)width)/100,((float)height)/100);
  }

  static void reshape(int w, int h){ 
    mWindows[currentWindow] -> reshape(w,h); 
    glViewport(0,0,w,h);
  }

  void onFrame(){
    SwapBuffers();
  }

  void setViewport(){
    glViewport(0,0, window().width(),window().height());  
   } 
};

Glut * Window::System;
vector<WindowData*> Window::mWindows;
int Window::currentWindow;


} //gfx::

#endif   /* ----- #ifndef GlutWindow_INC  ----- */
