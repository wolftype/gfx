/*
 * =====================================================================================
 *
 *       Filename:  GlutWindow.h
 *
 *    Description:  simple glut window for testing
 *
 *        Version:  1.0
 *        Created:  05/19/2014 19:13:15
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

#include <iostream>

#include <OpenGL/OpenGL.h>
#include <OpenGL/glu.h>
#include <GLUT/GLUT.h>

#include <stdio.h>
#include <vector>

using namespace std;


/*!
 *  Singleton initializer
 */
struct Glut {

  static Glut& Initialize(int argc, char ** argv){
    static Glut TheGlutInstance(argc,argv);
    return TheGlutInstance; 
  }

  static void Start(){
      glutMainLoop();
  }

  private:
    Glut(int argc, char ** argv){
      glutInit(&argc,argv);
      glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    }
};

/*!
 *  Stores width, height, and id of a Window
 */
struct WindowData {
  WindowData(int width = 100, int height = 100, int id = 0) : 
    mWidth(width), mHeight(height), mID(id){}
  int mWidth, mHeight, mID;
  float ratio() { return (float) mWidth/mHeight; }
  void reshape(int w, int h){
    mWidth = w; mHeight =h;
  }
  int width() const { return mWidth; }
  int height() const { return mHeight; }
};


/*!
 *  Window can have a bunch of windows, templated on anything that has an onDraw() method ...
 */
struct Window {
  
  static Glut * System;

  static void * app;
  static vector<WindowData*> mWindows;
  static int currentWindow;

  template<class APPLICATION>
  static void Bind(APPLICATION * _app) {
    app = _app;
  }

  template<class APPLICATION>
  static int Create(APPLICATION * _app, 
                    int w, int h, int offsetW = 0, int offsetH = 0){
    
    //Glut::Initialize(0,"");


    int id = glutCreateWindow("glut");
    cout <<"INITIALIZING WINDOW " << id << " " <<  w << " " << h << endl;  
       
    Bind(_app);                                   //<-- Bind App to void *
    
    glutDisplayFunc(Draw<APPLICATION>);           //<-- Callbacks Bind to Application
    //glutIdleFunc(Draw<APPLICATION>);
    glutTimerFunc(10, Animate<APPLICATION>, 1);
    glutReshapeFunc(Reshape<APPLICATION>);

    glutReshapeWindow(w,h);
    glutPositionWindow(offsetW, offsetH);

    mWindows.push_back( new WindowData(w,h,id) );
    currentWindow = id-1;
    return currentWindow;
  }

  static float ratio(){ 
    return mWindows[currentWindow] -> ratio(); 
  }
  static void reshape(int w, int h){ 
    mWindows[currentWindow] -> reshape(w,h); 
    glViewport(0,0,w,h);
  }

  static WindowData& window(){
    return *mWindows[currentWindow];
  }


  template<class APPLICATION>
  static void Animate(int){
    glutPostRedisplay();
    ((APPLICATION*)(app)) -> drawFunc();
    glutSwapBuffers();
    glutTimerFunc(10, Animate<APPLICATION>, 1);
  }


  template<class APPLICATION>
  static void Draw(void){
    ((APPLICATION*)(app)) -> drawFunc();
    glutSwapBuffers();
  }

  static void SwapBuffers(){
    glutSwapBuffers();
  }

  template<class APPLICATION>
  static void Reshape(int width, int height){
    currentWindow = glutGetWindow()-1;
    reshape(width,height);
    ((APPLICATION*)(app)) -> onResize(width,height);//((float)width)/100,((float)height)/100);
  }
};

Glut * Window::System;
void * Window::app;
vector<WindowData*> Window::mWindows;
int Window::currentWindow;




#endif   /* ----- #ifndef GlutWindow_INC  ----- */
