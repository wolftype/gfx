/*
 * =====================================================================================
 *
 *       Filename:  xGLFW.cpp
 *
 *    Description:  raw glfw
 *
 *        Version:  1.0
 *        Created:  08/05/2016 15:13:51
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail->wolftype
 *   Organization:  wolftype
 *
 * =====================================================================================
 */


#include <stdlib.h>
#define GLEW_STATIC
#include <GL/glew.h>
#include "GLFW/glfw3.h"


/*
 * =====================================================================================
 *
 *       Filename:  xWindow.cpp
 *
 *    Description:  Make a basic glut window from scratch and draw a 2D triangle in it
 *
 *        Version:  1.0
 *        Created:  06/09/2014 15:58:26
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *
 * =====================================================================================
 */

float width, height;
GLFWwindow * mWindow;

static void reshape(int w, int h){
  width  = w;
  height = h;
}


static void draw(void){

    //1. Set view port size and position
    glViewport(0,0,width,height);
    //2. Set the Clear Screen Background Color To Red
    glClearColor(1,0,0,1);
    //3. Clear the Screen
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //4. Transform The orthogonal Projection Matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-width/height,width/height, -1.f, 1.f, 1.f, -1.f);

    //5. Transform ModelView Matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //6. Draw a Triangle!
    glBegin(GL_TRIANGLES);

      glColor3f(1,0,0);
      glVertex3f(-1,-.5,0);
      glColor3f(0,1,0);
      glVertex3f(0,1,0);
      glColor3f(0,0,1);
      glVertex3f(1,-.5,0);

    glEnd();
}



int main(int argc, char * argv[] ){

    //0.set width and height
    width = 500;
    height = 500;

    //1. Initialize GLFW
    if( !glfwInit() ) exit(EXIT_FAILURE);

    //2. Create A Window
    mWindow = glfwCreateWindow(width,height,"test",NULL ,NULL);
    if (!mWindow) {
      glfwTerminate();
      exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(mWindow);

    glfwSwapInterval(1); //<-- force interval (not guaranteed to work with all graphics drivers)


    //7. Start The Main Loop
    while(!glfwWindowShouldClose(mWindow)){
        draw();

        glfwSwapBuffers(mWindow);
        glfwPollEvents();
    }

    //7. Destroy the Window
    glfwDestroyWindow(mWindow);

    //8. Terminate GLFW
    glfwTerminate();

    return 0;
}
