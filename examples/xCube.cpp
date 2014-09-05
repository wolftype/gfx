/*
 * =====================================================================================
 *
 *       Filename:  xCube.cpp
 *
 *    Description:  Draw a Cube!
 *
 *        Version:  1.0
 *        Created:  06/09/2014 14:04:08
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization:  
 *
 * =====================================================================================
 */

#include <iostream>
#include <stdlib.h>
#include <vector>

// INCLUDE the WINDOW Class From the Previous Lesson
#include "util/GlutWindow.h"

using namespace std;

// CREATE A PLAIN-OLD-DATA ("POD") Container for 3D Coordinates
struct Vec3 {
  float x,y,z;
};

//CREATE AN APPLICATION USING GLUT
struct App {

  Vec3 a,b,c,d,e,f,g,h;
  float time = 0.0;

  App() { init(); }

  void init(){

    //GLUTTY THINGS 
    Glut::Initialize();   
    Window::Create(this,640,480);

    //Specify the 8 VERTICES of CUBE
    a = { 1, -1,  1};               
    b = { 1,  1,  1}; 
    c = {-1,  1,  1}; 
    d = {-1, -1,  1}; 
    
    e = { 1, -1, -1}; 
    f = { 1,  1, -1}; 
    g = {-1,  1, -1}; 
    h = {-1, -1, -1}; 

   glEnable(GL_DEPTH_TEST);
   glDepthFunc(GL_LESS);
        
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   glPolygonMode( GL_BACK, GL_LINE );
        
   glLineWidth(5);
  }

  void start(){
    Glut::Start();
  }

  virtual void onResize(int w, int h){}
  
  virtual void drawFunc(){
    
    time += .05;                      //<-- INCREMENT COUNTER

    glViewport(0, 0, Window::window().width(), Window::window().height());
    glClearColor(1,0,0,1);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

   // glOrtho(-window.ratio, window.ratio, -1.f, 1.f, 1.f, -1.f);
    gluPerspective( 60, Window::window().ratio(),.1, 100);


    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity(); 
    
    gluLookAt( 
        0,0,5,                      //<-- Eye position (positive is TOWARDS you)
        0, 0, -1,                   //<-- Direction we are looking in (negative is into the screen)
        0, 1, 0);                   //<-- Which way is Up?      
    
    glRotatef( time, 0, 1, 0);      //<-- Rotate ModelView


       glBegin(GL_QUADS);              //<-- Start Rendering Rectangles

        /*-----------------------------------------------------------------------------
         *  FRONT FACE
         *-----------------------------------------------------------------------------*/
        
        glColor3f(1,0,0 );              //<-- Specify a COLOR in Red, Green, Blue (values in the range [0,1] )
       
        glVertex3f( a.x, a.y, a.z );    //<-- RENDER the Front Face
        glVertex3f( b.x, b.y, b.z );
        glVertex3f( c.x, c.y, c.z );
        glVertex3f( d.x, d.y, d.z );



        /*-----------------------------------------------------------------------------
         *  RIGHT SIDE FACE
         *-----------------------------------------------------------------------------*/

        glColor3f(0,1,0 );              //<-- Specify another COLOR (GREEN)

        glVertex3f( e.x, e.y, e.z );    //<-- Render the RIGHT SIDE Face
        glVertex3f( f.x, f.y, f.z );
        glVertex3f( b.x, b.y, b.z );
        glVertex3f( a.x, a.y, a.z );

        /*-----------------------------------------------------------------------------
         *  LEFT SIDE FACE
         *-----------------------------------------------------------------------------*/

        glColor3f(0,0,1 );              //<-- Specify another COLOR (BLUE)

        glVertex3f( d.x, d.y, d.z );    //<-- Render the LEFT SIDE Face
        glVertex3f( c.x, c.y, c.z );
        glVertex3f( g.x, g.y, g.z );
        glVertex3f( h.x, h.y, h.z );


        /*-----------------------------------------------------------------------------
         *  TOP FACE
         *-----------------------------------------------------------------------------*/

        glColor3f(1,1,0 );              //<-- Specify another COLOR (YELLOW)

        glVertex3f( b.x, b.y, b.z );    //<-- Render the TOP Face
        glVertex3f( f.x, f.y, f.z );
        glVertex3f( g.x, g.y, g.z );
        glVertex3f( c.x, c.y, c.z );

        /*-----------------------------------------------------------------------------
         *  BOTTOM FACE
         *-----------------------------------------------------------------------------*/

        glColor3f(0,1,1 );              //<-- Specify another COLOR (CYAN)

        glVertex3f( e.x, e.y, e.z );    //<-- Render the BOTTOM Face
        glVertex3f( a.x, a.y, a.z );
        glVertex3f( d.x, d.y, d.z );
        glVertex3f( h.x, h.y, h.z );


        /*-----------------------------------------------------------------------------
         *  BACK FACE
         *-----------------------------------------------------------------------------*/

        glColor3f(1,0,1 );              //<-- Specify another COLOR (PURPLE)

        glVertex3f( h.x, h.y, h.z );    //<-- Render the LEFT SIDE Face
        glVertex3f( g.x, g.y, g.z );
        glVertex3f( f.x, f.y, f.z );
        glVertex3f( e.x, e.y, e.z );


        glEnd();

        Window::SwapBuffers();              //<-- SWAP BUFFERS
 
  }

};


int main(int argc, const char ** argv){
 
 
  App app;

  app.start();
       
  return 0;

}
