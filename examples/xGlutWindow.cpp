/*
 * =====================================================================================
 *
 *       Filename:  xGlutWindow.cpp
 *
 *    Description:  using glut
 *
 *        Version:  1.0
 *        Created:  05/19/2014 16:37:19
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

struct WindowData {
  WindowData(int width = 100, int height = 100, id = 0) : 
    mWidth(width), mHeight(height), mID(id){}
  int mWidth, mHeight, mID;
  float ratio() { return (float) mWidth/mHeight; }
  void reshape(int w, int h){
    mWidth = w; mHeight =h;
  }
};

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

  static void Draw(void){

    glClearColor(1,.3,0,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();                      //<-- Load Identity Matrix
    gluPerspective(60, ratio(), 1, 100);   //<-- Lens Angle, Aspect Ratio, Near Clip, Far Clip

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();                     //<-- Load Identity Matrix
    gluLookAt( 0, 0, 1,                   //<-- Eye position (positive is TOWARDS you)
               0, 0,-1,                   //<-- Direction we are looking in (negative is into the screen)
               0, 1, 0);                  //<-- Which way is Up?   

    glBegin(GL_TRIANGLES);
      glColor3f(1,0,0); 
      glVertex3f(-.5,-.5,0);
      glColor3f(0,1,0);
      glVertex3f(0,.5,0);
      glColor3f(0,0,1);
      glVertex3f(.5,-.5,0); 
    glEnd();

    glutSwapBuffers();
  }

  static void Reshape(int width, int height){
    currentWindow = glutGetWindow()-1;
    reshape(width,height);
  }
};

vector<WindowData*> Window::mWindows;
int Window::currentWindow;

int main(int argc, char ** argv){

  glutInit(&argc,argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

  Window::Create(100,100,0,1000);
  Window::Create(100,100,500,100);

  glutMainLoop();

  return 0;
}


