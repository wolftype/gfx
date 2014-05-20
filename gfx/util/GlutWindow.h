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

#include <OpenGL/OpenGL.h>
#include <OpenGL/glu.h>
#include <GLUT/GLUT.h>

#include <stdio.h>
#include <vector>

using namespace std;


template<class T>
struct Drawable{
  
};


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
};


/*!
 *  Window can have a bunch of windows, templated on anything that has an onDraw() method ...
 */
template<class T>
struct Window {
    
  static vector<WindowData*> mWindows;
  static int currentWindow;

  static int Create(int w, int h, int offsetW = 0, int offsetH = 0){
    
    int id = glutCreateWindow("glut");
    
    glutDisplayFunc(Draw);
    glutReshapeFunc(Reshape);
    glutReshapeWindow(w,h);
    glutPositionWindow(offsetW, offsetH);

    mWindows.push_back( new WindowData(w,h,id) );
    currentWindow = id;
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

  static void Draw(void){

    glClearColor(1,.3,0,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    T::onDraw( window().mWidth, window().mHeight );

    glutSwapBuffers();
  }

  static void Reshape(int width, int height){
    currentWindow = glutGetWindow()-1;
    reshape(width,height);
  }
};

template<class T>
vector<WindowData*> Window<T>::mWindows;
template<class T>
int Window<T>::currentWindow;




